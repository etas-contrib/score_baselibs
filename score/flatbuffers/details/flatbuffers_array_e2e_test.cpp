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
#include "flatbuffers/array.h"

// flatc-generated see schema: details/array_cast_fixture.fbs
#include "score/flatbuffers/details/array_cast_fixture_generated.h"

#include <cstdint>
#include <cstring>
#include <limits>

#include "gtest/gtest.h"

namespace score
{

namespace flatbuffers
{

namespace test
{

using namespace ::flatbuffers;

// --------------------------------------------------------------------------------
// ArrayCastSafetyGeneratedTest
//
// End-to-end test for the generated header. See flatbuffers_serialization_safety.md
// and details/array_cast_fixture.fbs for the schema.
// --------------------------------------------------------------------------------

namespace
{
// Distinct 64-bit values for the [int64:2] field, chosen to also exercise the
// full 64-bit range so a truncated/endian-wrong reinterpret view would be caught.
constexpr int64_t kFirst64 = std::numeric_limits<int64_t>::min();
constexpr int64_t kSecond64 = std::numeric_limits<int64_t>::max() - 7;
}  // namespace

// End-to-end: build a serialized buffer through the generated, typed API and
// read every field back through the reinterpret-cast Array view.
TEST(ArrayCastSafetyGeneratedTest, EndToEndRoundTripThroughGeneratedBuffer)
{
    RecordProperty("PartiallyVerifies", "comp_req__flatbuffers__serialization");
    RecordProperty("Description", "reinterpret-cast Array view read from a serialized buffer matches written values");
    RecordProperty("TestType", "interface-test");
    RecordProperty("DerivationTechnique", "requirements-analysis");

    const int32_t coordinate_values[2] = {10, 20};
    const arrays_fixture::TestEnum channel_mode_values[2] = {arrays_fixture::TestEnum::B, arrays_fixture::TestEnum::C};
    const int64_t timestamp_values[2] = {kFirst64, kSecond64};

    FlatBufferBuilder fbb;
    const arrays_fixture::NestedStruct nested(span<const int32_t, 2>(coordinate_values, 2),
                                              arrays_fixture::TestEnum::A,
                                              span<const arrays_fixture::TestEnum, 2>(channel_mode_values, 2),
                                              span<const int64_t, 2>(timestamp_values, 2));
    fbb.Finish(arrays_fixture::CreateArraysHolder(fbb, &nested));

    // The produced bytes are a verifiable FlatBuffer
    Verifier verifier(fbb.GetBufferPointer(), fbb.GetSize());
    ASSERT_TRUE(arrays_fixture::VerifyArraysHolderBuffer(verifier));

    const arrays_fixture::ArraysHolder* holder = arrays_fixture::GetArraysHolder(fbb.GetBufferPointer());
    ASSERT_NE(holder, nullptr);
    const arrays_fixture::NestedStruct* view = holder->nested();
    ASSERT_NE(view, nullptr);

    // coordinates(): [int:2] read back through CastToArray.
    ASSERT_NE(view->coordinates(), nullptr);
    EXPECT_EQ(view->coordinates()->size(), 2u);
    EXPECT_EQ(view->coordinates()->Get(0), 10);
    EXPECT_EQ(view->coordinates()->Get(1), 20);

    // status(): scalar enum.
    EXPECT_EQ(view->status(), arrays_fixture::TestEnum::A);

    // channel_modes(): [TestEnum:2] read back through CastToArrayOfEnum.
    ASSERT_NE(view->channel_modes(), nullptr);
    EXPECT_EQ(view->channel_modes()->size(), 2u);
    EXPECT_EQ(view->channel_modes()->Get(0), arrays_fixture::TestEnum::B);
    EXPECT_EQ(view->channel_modes()->Get(1), arrays_fixture::TestEnum::C);

    // timestamps(): [int64:2] read back through CastToArray, full 64-bit values preserved.
    ASSERT_NE(view->timestamps(), nullptr);
    EXPECT_EQ(view->timestamps()->size(), 2u);
    EXPECT_EQ(view->timestamps()->Get(0), kFirst64);
    EXPECT_EQ(view->timestamps()->Get(1), kSecond64);
}

// Measure #2 (const-correctness): read accessors return const Array<T,N>*, so the
// aliased buffer cannot be mutated through the view.
TEST(ArrayCastSafetyGeneratedTest, ReadAccessorsAreConstCorrect)
{
    RecordProperty("PartiallyVerifies", "comp_req__flatbuffers__serialization");
    RecordProperty("Description",
                   "coordinates()/channel_modes()/timestamps() return const Array<T,N>* so the view is read-only");
    RecordProperty("TestType", "interface-test");
    RecordProperty("DerivationTechnique", "requirements-analysis");

    static_assert(std::is_same<decltype(std::declval<const arrays_fixture::NestedStruct>().coordinates()),
                               const Array<int32_t, 2>*>::value,
                  "coordinates() must return const Array<int32_t, 2>*");
    static_assert(std::is_same<decltype(std::declval<const arrays_fixture::NestedStruct>().channel_modes()),
                               const Array<arrays_fixture::TestEnum, 2>*>::value,
                  "channel_modes() must return const Array<TestEnum, 2>*");
    static_assert(std::is_same<decltype(std::declval<const arrays_fixture::NestedStruct>().timestamps()),
                               const Array<int64_t, 2>*>::value,
                  "timestamps() must return const Array<int64_t, 2>*");
    SUCCEED();
}

// Measure #3 (enum type safety): enum arrays route through CastToArrayOfEnum<E>,
// whose static_assert(sizeof(E) == sizeof(T)) makes a mismatched storage type a
// compile error. Verify the generated enum's storage matches its array element.
TEST(ArrayCastSafetyGeneratedTest, EnumArrayStorageTypeIsSizeChecked)
{
    RecordProperty("PartiallyVerifies", "comp_req__flatbuffers__serialization");
    RecordProperty("Description", "TestEnum underlying storage equals its int8_t backing array element size");
    RecordProperty("TestType", "interface-test");
    RecordProperty("DerivationTechnique", "requirements-analysis");

    static_assert(std::is_same<std::underlying_type<arrays_fixture::TestEnum>::type, int8_t>::value,
                  "TestEnum must be backed by int8_t");
    static_assert(sizeof(arrays_fixture::TestEnum) == sizeof(int8_t),
                  "enum size must match its storage type -- the CastToArrayOfEnum precondition");
    SUCCEED();
}

// Measure #4 (write-size safety): the only write path is the constructor taking
// fixed-extent spans, so a wrong-length write does not compile. Prove it: the
// struct is constructible from correct-extent spans but not from a wrong extent.
TEST(ArrayCastSafetyGeneratedTest, WriteExtentIsCompileTimeChecked)
{
    RecordProperty("PartiallyVerifies", "comp_req__flatbuffers__serialization");
    RecordProperty("Description", "constructor binds compile-time extent-2 spans; a wrong-length span is rejected");
    RecordProperty("TestType", "interface-test");
    RecordProperty("DerivationTechnique", "boundary-values");

    static_assert(std::is_constructible<arrays_fixture::NestedStruct,
                                        span<const int32_t, 2>,
                                        arrays_fixture::TestEnum,
                                        span<const arrays_fixture::TestEnum, 2>,
                                        span<const int64_t, 2>>::value,
                  "correct fixed extents must construct");
    static_assert(!std::is_constructible<arrays_fixture::NestedStruct,
                                         span<const int32_t, 3>,  // wrong extent for coordinates:[int:2]
                                         arrays_fixture::TestEnum,
                                         span<const arrays_fixture::TestEnum, 2>,
                                         span<const int64_t, 2>>::value,
                  "a wrong-length span must not construct");
    static_assert(!std::is_constructible<arrays_fixture::NestedStruct,
                                         span<const int32_t, 2>,
                                         arrays_fixture::TestEnum,
                                         span<const arrays_fixture::TestEnum, 2>,
                                         span<const int64_t, 1>>::value,  // wrong extent for timestamps:[int64:2]
                  "a wrong-length span must not construct");
    SUCCEED();
}

// Measure #5 (no uninitialized reads): every constructor zero-initializes padding.
// Construct the struct over two differently poisoned memory regions; because every
// byte -- data and padding alike -- is written deterministically, the two results
// must be byte-identical. Leftover garbage in any padding byte would fail this.
TEST(ArrayCastSafetyGeneratedTest, PaddingIsDeterministicallyZeroInitialized)
{
    RecordProperty("PartiallyVerifies", "comp_req__flatbuffers__serialization");
    RecordProperty("Description", "construction fully determines all bytes, leaving no uninitialized padding");
    RecordProperty("TestType", "interface-test");
    RecordProperty("DerivationTechnique", "requirements-analysis");

    alignas(arrays_fixture::NestedStruct) unsigned char buf_a[sizeof(arrays_fixture::NestedStruct)];
    alignas(arrays_fixture::NestedStruct) unsigned char buf_b[sizeof(arrays_fixture::NestedStruct)];
    std::memset(buf_a, 0xAA, sizeof(buf_a));
    std::memset(buf_b, 0x55, sizeof(buf_b));

    const int32_t coordinate_values[2] = {10, 20};
    const arrays_fixture::TestEnum channel_mode_values[2] = {arrays_fixture::TestEnum::A, arrays_fixture::TestEnum::B};
    const int64_t timestamp_values[2] = {kFirst64, kSecond64};

    const auto construct = [&](void* where) {
        new (where) arrays_fixture::NestedStruct(span<const int32_t, 2>(coordinate_values, 2),
                                                 arrays_fixture::TestEnum::C,
                                                 span<const arrays_fixture::TestEnum, 2>(channel_mode_values, 2),
                                                 span<const int64_t, 2>(timestamp_values, 2));
    };
    construct(buf_a);
    construct(buf_b);

    EXPECT_EQ(std::memcmp(buf_a, buf_b, sizeof(arrays_fixture::NestedStruct)), 0);
}

}  // namespace test
}  // namespace flatbuffers
}  // namespace score
