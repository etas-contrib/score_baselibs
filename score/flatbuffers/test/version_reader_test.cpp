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

#include "score/flatbuffers/version_reader.hpp"
#include "score/filesystem/path.h"
#include "score/flatbuffers/buffer_version_info.hpp"
#include "score/flatbuffers/i_version_reader.hpp"
#include "score/flatbuffers/load_buffer.hpp"

#include <gtest/gtest.h>

#include <cstdint>
#include <cstdlib>
#include <string>
#include <vector>

/// Integration test for `VersionReader` / `IVersionReader`.
///
///   - The test fixture owns a `score::flatbuffers::VersionReader` (concrete
///     implementation) as a member and exposes it via the abstract
///     `IVersionReader&` reference.
///   - Each test calls the `IVersionReader` interface and the free function.
///   - The overload with the vector parameter (buf) is tested since it internally
///     delegates to the pointer/size version.
///
class VersionReaderTest : public ::testing::Test
{

  protected:
    void SetUp() override
    {
        std::string bin_path{"score/flatbuffers/test/testdata/versioned_buffer.bin"};
        auto load_result = score::flatbuffers::LoadBuffer(score::filesystem::Path{bin_path});
        ASSERT_TRUE(load_result.has_value()) << "LoadBuffer failed: " << bin_path;
        data_ = std::move(load_result).value();
    }

    std::vector<uint8_t> data_;

    /// Concrete implementation – owned by the fixture, lifetime matches the test.
    score::flatbuffers::VersionReader reader_;

    /// All tests access functionality through the abstract interface,
    /// mirroring how production code should depend on IVersionReader.
    score::flatbuffers::IVersionReader& version_reader_{reader_};
};

TEST_F(VersionReaderTest, GetVersionRejectsNullPointer)
{
    RecordProperty("PartiallyVerifies", "comp_req__flatbuffers__buffer_identification");
    RecordProperty("TestType", "fault-injection");
    RecordProperty("DerivationTechnique", "equivalence-classes");
    RecordProperty("Description", "kNullDataPointer is returned if null pointer is passed instead of buffer data");

    const auto result = version_reader_.GetVersion(nullptr, 64U);
    ASSERT_FALSE(result.has_value());
    EXPECT_EQ(result.error(), MakeError(score::flatbuffers::ErrorCode::kNullDataPointer));

    const auto result2 = score::flatbuffers::GetVersion(nullptr, 64U);
    ASSERT_FALSE(result2.has_value());
    EXPECT_EQ(result2.error(), MakeError(score::flatbuffers::ErrorCode::kNullDataPointer));
}

TEST_F(VersionReaderTest, GetVersionRejectsEmptyBuffer)
{
    RecordProperty("PartiallyVerifies", "comp_req__flatbuffers__buffer_identification");
    RecordProperty("TestType", "fault-injection");
    RecordProperty("DerivationTechnique", "equivalence-classes");
    RecordProperty("Description", "kVerificationFailed is returned if buffer is empty");

    const auto result = version_reader_.GetVersion(std::vector<uint8_t>{});
    EXPECT_FALSE(result.has_value());
    EXPECT_EQ(result.error(), MakeError(score::flatbuffers::ErrorCode::kVerificationFailed));

    const auto result2 = score::flatbuffers::GetVersion(std::vector<uint8_t>{});
    EXPECT_FALSE(result2.has_value());
    EXPECT_EQ(result2.error(), MakeError(score::flatbuffers::ErrorCode::kVerificationFailed));
}

TEST_F(VersionReaderTest, GetVersionRejectsTooSmallBuffer)
{
    RecordProperty("PartiallyVerifies", "comp_req__flatbuffers__buffer_identification");
    RecordProperty("TestType", "fault-injection");
    RecordProperty("DerivationTechnique", "equivalence-classes");
    RecordProperty("Description", "kVerificationFailed is returned if buffer is too small to contain version info");

    const std::vector<uint8_t> tiny_buf = {0, 0, 0, 0};
    const auto result = version_reader_.GetVersion(tiny_buf);
    EXPECT_FALSE(result.has_value());
    EXPECT_EQ(result.error(), MakeError(score::flatbuffers::ErrorCode::kVerificationFailed));

    const auto result2 = score::flatbuffers::GetVersion(tiny_buf);
    EXPECT_FALSE(result2.has_value());
    EXPECT_EQ(result2.error(), MakeError(score::flatbuffers::ErrorCode::kVerificationFailed));
}

TEST_F(VersionReaderTest, GetVersionRejectsInvalidBuffer)
{
    RecordProperty("PartiallyVerifies", "comp_req__flatbuffers__buffer_identification");
    RecordProperty("TestType", "fault-injection");
    RecordProperty("DerivationTechnique", "equivalence-classes");
    RecordProperty("Description", "kVerificationFailed is returned if buffer does not contain valid version info");

    const std::vector<uint8_t> bad_buf(64, 0);
    const auto result = version_reader_.GetVersion(bad_buf);
    EXPECT_FALSE(result.has_value());
    EXPECT_EQ(result.error(), MakeError(score::flatbuffers::ErrorCode::kVerificationFailed));

    const auto result2 = score::flatbuffers::GetVersion(bad_buf);
    EXPECT_FALSE(result2.has_value());
    EXPECT_EQ(result2.error(), MakeError(score::flatbuffers::ErrorCode::kVerificationFailed));
}

// ---------------------------------------------------------------------------
// Parameterized fixture for wrong-field placement buffers:
// ---------------------------------------------------------------------------
struct WrongFieldParam
{
    std::string bin_path;
    std::string name;  // used as the GoogleTest instance suffix
};

class VersionReaderWrongFieldTest : public ::testing::TestWithParam<WrongFieldParam>
{
  protected:
    score::flatbuffers::VersionReader reader_;
    score::flatbuffers::IVersionReader& version_reader_{reader_};
};

// clang-format off
INSTANTIATE_TEST_SUITE_P(
    WrongFieldBuffers,
    VersionReaderWrongFieldTest,
    ::testing::Values(
        WrongFieldParam{
            "score/flatbuffers/test/testdata/versioned_wrong_field.bin",
            "WrongField"
        },
        WrongFieldParam{
            "score/flatbuffers/test/testdata/versioned_wrong_field_similar_type.bin",
            "WrongFieldSimilarType"
        }
    ),
    [](const ::testing::TestParamInfo<WrongFieldParam>& info) { return info.param.name; }
);
// clang-format on

TEST_P(VersionReaderWrongFieldTest, GetVersionRejectsBufferWithVersionInfoInWrongField)
{
    RecordProperty("PartiallyVerifies", "comp_req__flatbuffers__buffer_identification");
    RecordProperty("TestType", "fault-injection");
    RecordProperty("DerivationTechnique", "equivalence-classes");
    RecordProperty("Description",
                   "kVerificationFailed is returned if version_info is not in field 0 of the root table");

    const auto& param = GetParam();
    auto load_result = score::flatbuffers::LoadBuffer(score::filesystem::Path{param.bin_path});
    ASSERT_TRUE(load_result.has_value()) << "LoadBuffer failed for " << param.bin_path;
    const auto& wrong_field_buf = load_result.value();

    const auto result = version_reader_.GetVersion(wrong_field_buf);
    ASSERT_FALSE(result.has_value());
    EXPECT_EQ(result.error(), MakeError(score::flatbuffers::ErrorCode::kVerificationFailed));

    const auto result2 = score::flatbuffers::GetVersion(wrong_field_buf);
    ASSERT_FALSE(result2.has_value());
    EXPECT_EQ(result2.error(), MakeError(score::flatbuffers::ErrorCode::kVerificationFailed));
}

// ---------------------------------------------------------------------------
// Parameterized fixture for version verification
// ---------------------------------------------------------------------------
struct VersionBufferParam
{
    std::string bin_path;
    score::flatbuffers::BufferVersionInfo version_info;
    std::string name;  // used as the GoogleTest instance suffix
};

class VersionReaderBufferParamTest : public ::testing::TestWithParam<VersionBufferParam>
{
  protected:
    void SetUp() override
    {
        const auto& param = GetParam();
        auto load_result = score::flatbuffers::LoadBuffer(score::filesystem::Path{param.bin_path});
        ASSERT_TRUE(load_result.has_value()) << "LoadBuffer failed: " << param.bin_path;
        data_ = std::move(load_result).value();
    }

    std::vector<uint8_t> data_;
    score::flatbuffers::VersionReader reader_;
    score::flatbuffers::IVersionReader& version_reader_{reader_};
};

// clang-format off
INSTANTIATE_TEST_SUITE_P(
    AllVersionBuffers,
    VersionReaderBufferParamTest,
    ::testing::Values(
        VersionBufferParam{"score/flatbuffers/test/testdata/versioned_buffer.bin",           {"DEMO",  2U,     3U},     "Normal"},
        VersionBufferParam{"score/flatbuffers/test/testdata/versioned_min_buffer.bin",       {"    ",  0U,     0U},     "MinVersion"},
        VersionBufferParam{"score/flatbuffers/test/testdata/versioned_min_range_buffer.bin", {" !\"#", 0U,     1U},     "MinRangeVersion"},
        VersionBufferParam{"score/flatbuffers/test/testdata/versioned_max_range_buffer.bin", {"{|}~",  UINT16_MAX, 65534U}, "MaxRangeVersion"},
        VersionBufferParam{"score/flatbuffers/test/testdata/versioned_max_buffer.bin",       {"~~~~",  UINT16_MAX, UINT16_MAX}, "MaxVersion"}
    ),
    [](const ::testing::TestParamInfo<VersionBufferParam>& info) { return info.param.name; }
);
// clang-format on

TEST_P(VersionReaderBufferParamTest, ReadBufferVersionFromBinary)
{
    RecordProperty("PartiallyVerifies", "comp_req__flatbuffers__buffer_identification");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "boundary-values");
    RecordProperty("Description", "version info is correctly read from buffer");

    const auto& param = GetParam();
    const auto result = version_reader_.GetVersion(data_);

    ASSERT_TRUE(result.has_value()) << "GetVersion failed";

    const auto& info = result.value();

    EXPECT_EQ(info.identifier(), param.version_info.identifier());
    EXPECT_EQ(info.major_version, param.version_info.major_version);
    EXPECT_EQ(info.minor_version, param.version_info.minor_version);
}

TEST_P(VersionReaderBufferParamTest, VerifyVersionSucceedsOnMatch)
{
    RecordProperty("PartiallyVerifies", "comp_req__flatbuffers__version_check");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "boundary-values");
    RecordProperty("Description", "VerifyVersion returns success if version info matches expected");

    const auto& param = GetParam();

    const auto result = version_reader_.VerifyVersion(data_, param.version_info);
    EXPECT_TRUE(result.has_value());

    const auto result2 = score::flatbuffers::VerifyVersion(data_, param.version_info);
    EXPECT_TRUE(result2.has_value());
}

TEST_P(VersionReaderBufferParamTest, VerifyVersionFailsOnVersionMismatch)
{
    RecordProperty("PartiallyVerifies", "comp_req__flatbuffers__version_check");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "boundary-values");
    RecordProperty("Description", "VerifyVersion returns error if version info does not match expected");

    score::flatbuffers::BufferVersionInfo expected = GetParam().version_info;
    expected.major_version = 99U;  // wrong major version
    expected.minor_version = 1U;   // wrong minor version

    const auto result = version_reader_.VerifyVersion(data_, expected);
    EXPECT_FALSE(result.has_value());

    const auto result2 = score::flatbuffers::VerifyVersion(data_, expected);
    EXPECT_FALSE(result2.has_value());
}

TEST_P(VersionReaderBufferParamTest, VerifyVersionFailsOnIdentifierMismatch)
{
    RecordProperty("PartiallyVerifies", "comp_req__flatbuffers__version_check");
    RecordProperty("TestType", "requirements-based");
    RecordProperty("DerivationTechnique", "requirements-analysis");
    RecordProperty("Description", "VerifyVersion returns error if identifier does not match expected");

    // Use the correct major/minor version but wrong identifier
    const auto& param = GetParam();
    score::flatbuffers::BufferVersionInfo expected = {"XXXX", 0, 0};
    expected.major_version = param.version_info.major_version;
    expected.minor_version = param.version_info.minor_version;

    const auto result = version_reader_.VerifyVersion(data_, expected);
    EXPECT_FALSE(result.has_value());

    const auto result2 = score::flatbuffers::VerifyVersion(data_, expected);
    EXPECT_FALSE(result2.has_value());
}

TEST_P(VersionReaderBufferParamTest, VerifyVersionMinorMinimumSucceedsWhenActualMinorIsHigher)
{
    RecordProperty("PartiallyVerifies", "comp_req__flatbuffers__version_check");
    RecordProperty("TestType", "interface-test");
    RecordProperty("DerivationTechnique", "equivalence-classes");
    RecordProperty("Description",
                   "VerifyVersion with kMinorMinimum returns success if actual minor version is higher than expected");

    const auto& param = GetParam();
    score::flatbuffers::BufferVersionInfo expected = param.version_info;
    // For the minimum version buffers we can only proove minor==0.
    // All other will confirm that minor>=0 and therefore valid.
    expected.minor_version = 0;

    const auto result =
        version_reader_.VerifyVersion(data_, expected, score::flatbuffers::VersionMatchMode::kMinorMinimum);
    EXPECT_TRUE(result.has_value());

    const auto result2 =
        score::flatbuffers::VerifyVersion(data_, expected, score::flatbuffers::VersionMatchMode::kMinorMinimum);
    EXPECT_TRUE(result2.has_value());
}

TEST_P(VersionReaderBufferParamTest, VerifyVersionMinorMinimumFailsWhenActualMinorIsLower)
{
    RecordProperty("PartiallyVerifies", "comp_req__flatbuffers__version_check");
    RecordProperty("TestType", "interface-test");
    RecordProperty("DerivationTechnique", "equivalence-classes");
    RecordProperty("Description",
                   "VerifyVersion with kMinorMinimum returns error if actual minor version is lower than expected");

    const auto& param = GetParam();
    score::flatbuffers::BufferVersionInfo expected = param.version_info;
    expected.minor_version = UINT16_MAX;

    const auto result =
        version_reader_.VerifyVersion(data_, expected, score::flatbuffers::VersionMatchMode::kMinorMinimum);
    if (param.version_info.minor_version == UINT16_MAX)
    {
        // If the actual minor version is at maximum we can't expect a higher value
        // Test fallbacks to expected minor == actual (UINT16_MAX) which is valid.
        EXPECT_TRUE(result.has_value());
    }
    else
    {
        EXPECT_FALSE(result.has_value());
    }

    const auto result2 =
        score::flatbuffers::VerifyVersion(data_, expected, score::flatbuffers::VersionMatchMode::kMinorMinimum);
    if (param.version_info.minor_version == UINT16_MAX)
    {
        // If the actual minor version is at maximum we can't expect a higher value
        // Test fallbacks to expected minor == actual (UINT16_MAX) which is valid.
        EXPECT_TRUE(result2.has_value());
    }
    else
    {
        EXPECT_FALSE(result2.has_value());
    }
}
