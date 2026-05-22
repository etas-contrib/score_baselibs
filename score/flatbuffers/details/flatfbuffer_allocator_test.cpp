/********************************************************************************
 * Copyright (c) 2026 Contributors to the Eclipse Foundation
 *
 * See the NOTICE file(s) distributed with this work for additional
 * information regarding copyright ownership.
 *
 * This program and the accompanying materials are made available under the
 * terms of the Apache License Version 2.0 which is available at
 * https://www.apache.org/licenses/LICENSE-2.0
 *
 * SPDX-License-Identifier: Apache-2.0
 ********************************************************************************/
#include "flatbuffers/allocator.h"
#include "gcov_dump_on_abort.hpp"

#include <cstdint>
#include <cstring>
#include <memory>

#include "gtest/gtest.h"

namespace score
{

namespace flatbuffers
{

namespace unit_test
{

// Magic-number sentinels used across tests.
constexpr uint8_t kSentinelFront = 0x0F;
constexpr uint8_t kSentinelFill = 0x55;
constexpr uint8_t kSentinelWrite = 0xAB;
constexpr uint8_t kSentinelBack = 0x80;

// ---------------------------------------------------------------------------
// Minimal concrete Allocator implementation used across all tests.
// ---------------------------------------------------------------------------
class SimpleAllocator : public ::flatbuffers::Allocator
{
  public:
    // NOLINTNEXTLINE(cppcoreguidelines-owning-memory)
    uint8_t* allocate(size_t size) override
    {
        return new uint8_t[size];  // NOLINT(cppcoreguidelines-owning-memory)
    }

    void deallocate(uint8_t* ptr, size_t /*size*/) override
    {
        // NOLINTNEXTLINE(cppcoreguidelines-owning-memory)
        delete[] ptr;
    }
};

// ---------------------------------------------------------------------------
// AllocatorConcreteTest
// Tests that a concrete subclass satisfies the Allocator contract.
// ---------------------------------------------------------------------------

// Function under test: Allocator::allocate, Allocator::deallocate
// Equivalence class: allocate returns non-null; deallocate does not crash.
TEST(AllocatorConcreteTest, AllocateReturnsNonNull)
{
    EXPECT_NO_THROW({
        SimpleAllocator alloc;
        constexpr size_t kSize = 64U;
        uint8_t* ptr = alloc.allocate(kSize);
        ASSERT_NE(ptr, nullptr);
        alloc.deallocate(ptr, kSize);
    });
}

// Function under test: Allocator::allocate, Allocator::deallocate
// Boundary: allocate a single byte.
TEST(AllocatorConcreteTest, AllocateSingleByte)
{
    SimpleAllocator alloc;
    uint8_t* ptr = alloc.allocate(1U);
    ASSERT_NE(ptr, nullptr);
    *ptr = kSentinelWrite;
    EXPECT_EQ(*ptr, kSentinelWrite);
    alloc.deallocate(ptr, 1U);
}

// Function under test: Allocator::allocate, Allocator::deallocate
// Boundary: allocate a large region.
TEST(AllocatorConcreteTest, AllocateLargeRegion)
{
    SimpleAllocator alloc;
    constexpr size_t kSize = static_cast<size_t>(1024U) * 1024U;  // 1 MiB
    uint8_t* ptr = alloc.allocate(kSize);
    ASSERT_NE(ptr, nullptr);
    std::memset(ptr, kSentinelFill, kSize);
    // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-pointer-arithmetic)
    EXPECT_EQ(ptr[0], kSentinelFill);
    // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-pointer-arithmetic)
    EXPECT_EQ(ptr[kSize - 1U], kSentinelFill);
    alloc.deallocate(ptr, kSize);
}

// Function under test: Allocator::allocate, Allocator::deallocate, ~Allocator
// Equivalence class: allocator is usable through base-class pointer.
TEST(AllocatorConcreteTest, UsableThroughBasePointer)
{
    constexpr size_t kSize = 32U;
    auto alloc = std::make_unique<SimpleAllocator>();
    uint8_t* ptr = alloc->allocate(kSize);
    ASSERT_NE(ptr, nullptr);
    alloc->deallocate(ptr, kSize);
    // unique_ptr exercises virtual destructor on scope exit
}

// ---------------------------------------------------------------------------
// AllocatorReallocateDownwardTest
// Tests the default reallocate_downward implementation provided by Allocator.
// ---------------------------------------------------------------------------

// Function under test: Allocator::reallocate_downward
// Equivalence class: reallocate_downward grows the buffer and returns non-null.
TEST(AllocatorReallocateDownwardTest, GrowsBuffer)
{
    SimpleAllocator alloc;
    constexpr size_t kOldSize = 64U;
    constexpr size_t kNewSize = 128U;

    uint8_t* old_ptr = alloc.allocate(kOldSize);
    ASSERT_NE(old_ptr, nullptr);

    // No data in use — in_use_back and in_use_front are both 0.
    uint8_t* new_ptr = alloc.reallocate_downward(old_ptr, kOldSize, kNewSize, 0U, 0U);
    ASSERT_NE(new_ptr, nullptr);
    EXPECT_NE(new_ptr, old_ptr);

    alloc.deallocate(new_ptr, kNewSize);
}

// Function under test: Allocator::reallocate_downward, memcpy_downward
// Equivalence class: memcpy_downward preserves back data across reallocation.
// The `in_use_back` bytes at the end of `old_ptr` are copied to the end of `new_ptr`.
TEST(AllocatorReallocateDownwardTest, PreservesBackData)
{
    SimpleAllocator alloc;
    constexpr size_t kOldSize = 64U;
    constexpr size_t kNewSize = 128U;
    constexpr size_t kBack = 16U;  // bytes at the tail in use

    uint8_t* old_ptr = alloc.allocate(kOldSize);
    ASSERT_NE(old_ptr, nullptr);

    // Write a known pattern into the back (tail) region.
    for (size_t i = 0U; i < kBack; ++i)
    {
        // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-pointer-arithmetic)
        old_ptr[kOldSize - kBack + i] = static_cast<uint8_t>(i + 1U);
    }

    uint8_t* new_ptr = alloc.reallocate_downward(old_ptr, kOldSize, kNewSize, kBack, 0U);
    ASSERT_NE(new_ptr, nullptr);

    // Back data should appear at the tail of the new buffer.
    for (size_t i = 0U; i < kBack; ++i)
    {
        // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-pointer-arithmetic)
        EXPECT_EQ(new_ptr[kNewSize - kBack + i], static_cast<uint8_t>(i + 1U)) << "mismatch at back index " << i;
    }

    alloc.deallocate(new_ptr, kNewSize);
}

// Function under test: Allocator::reallocate_downward, memcpy_downward
// Equivalence class: memcpy_downward preserves front data across reallocation.
// The `in_use_front` bytes at the start of `old_ptr` are copied to the start of `new_ptr`.
TEST(AllocatorReallocateDownwardTest, PreservesFrontData)
{
    SimpleAllocator alloc;
    constexpr size_t kOldSize = 64U;
    constexpr size_t kNewSize = 128U;
    constexpr size_t kFront = 8U;  // bytes at the head in use

    uint8_t* old_ptr = alloc.allocate(kOldSize);
    ASSERT_NE(old_ptr, nullptr);

    // Write a known pattern into the front region.
    for (size_t i = 0U; i < kFront; ++i)
    {
        // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-pointer-arithmetic)
        old_ptr[i] = static_cast<uint8_t>(kSentinelFront + i);
    }

    uint8_t* new_ptr = alloc.reallocate_downward(old_ptr, kOldSize, kNewSize, 0U, kFront);
    ASSERT_NE(new_ptr, nullptr);

    // Front data should appear at the start of the new buffer.
    for (size_t i = 0U; i < kFront; ++i)
    {
        // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-pointer-arithmetic)
        EXPECT_EQ(new_ptr[i], static_cast<uint8_t>(kSentinelFront + i)) << "mismatch at front index " << i;
    }

    alloc.deallocate(new_ptr, kNewSize);
}

// Function under test: Allocator::reallocate_downward, memcpy_downward
// Boundary: front and back data are both preserved simultaneously.
TEST(AllocatorReallocateDownwardTest, PreservesBothFrontAndBackData)
{
    SimpleAllocator alloc;
    constexpr size_t kOldSize = 64U;
    constexpr size_t kNewSize = 256U;
    constexpr size_t kBack = 20U;
    constexpr size_t kFront = 12U;

    uint8_t* old_ptr = alloc.allocate(kOldSize);
    ASSERT_NE(old_ptr, nullptr);

    for (size_t i = 0U; i < kFront; ++i)
    {
        // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-pointer-arithmetic)
        old_ptr[i] = static_cast<uint8_t>(kSentinelFront + i);
    }
    for (size_t i = 0U; i < kBack; ++i)
    {
        // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-pointer-arithmetic)
        old_ptr[kOldSize - kBack + i] = static_cast<uint8_t>(kSentinelBack + i);
    }

    uint8_t* new_ptr = alloc.reallocate_downward(old_ptr, kOldSize, kNewSize, kBack, kFront);
    ASSERT_NE(new_ptr, nullptr);

    for (size_t i = 0U; i < kFront; ++i)
    {
        // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-pointer-arithmetic)
        EXPECT_EQ(new_ptr[i], static_cast<uint8_t>(kSentinelFront + i)) << "front mismatch at index " << i;
    }
    for (size_t i = 0U; i < kBack; ++i)
    {
        // NOLINTNEXTLINE(cppcoreguidelines-pro-bounds-pointer-arithmetic)
        EXPECT_EQ(new_ptr[kNewSize - kBack + i], static_cast<uint8_t>(kSentinelBack + i))
            << "back mismatch at index " << i;
    }

    alloc.deallocate(new_ptr, kNewSize);
}

// Function under test: Allocator::reallocate_downward
// Boundary: reallocate_downward asserts when new_size == old_size.
// Fault cause: FLATBUFFERS_ASSERT(new_size > old_size) fires for equal sizes.
TEST(AllocatorReallocateDownwardTest, AssertsWhenNewSizeEqualsOldSize)
{
    EXPECT_DEATH(
        {
            InstallGcovDumpOnAbort();
            SimpleAllocator alloc;
            constexpr size_t kSize = 64U;
            uint8_t* ptr = alloc.allocate(kSize);
            alloc.reallocate_downward(ptr, kSize, kSize, 0U, 0U);
        },
        "");
}

// Function under test: Allocator::reallocate_downward
// Boundary: reallocate_downward asserts when new_size < old_size.
// Fault cause: FLATBUFFERS_ASSERT(new_size > old_size) fires for shrinking.
TEST(AllocatorReallocateDownwardTest, AssertsWhenNewSizeSmallerThanOldSize)
{
    EXPECT_DEATH(
        {
            InstallGcovDumpOnAbort();
            SimpleAllocator alloc;
            constexpr size_t kOldSize = 64U;
            constexpr size_t kNewSize = 32U;
            uint8_t* ptr = alloc.allocate(kOldSize);
            alloc.reallocate_downward(ptr, kOldSize, kNewSize, 0U, 0U);
        },
        "");
}

}  // namespace unit_test
}  // namespace flatbuffers
}  // namespace score
