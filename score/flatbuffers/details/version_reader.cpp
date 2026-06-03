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
#include "score/flatbuffers/common/buffer_version_envelope_generated.h"

#include "flatbuffers/base.h"

#include <cstddef>
#include <cstdint>
#include <cstring>
#include <vector>

namespace score
{
namespace flatbuffers
{

// ─── Implementation ──────────────────────────────────────────────────────────

namespace
{

template <auto GetEnvelope = ::score::flatbuffers::GetBufferVersionEnvelope>
inline score::Result<BufferVersionInfo> GetVersionImpl(const uint8_t* data, const std::size_t size) noexcept
{
    if (data == nullptr)
    {
        return MakeUnexpected(ErrorCode::kNullDataPointer);
    }

    // Structural integrity check using the generated Verifier for BufferVersionEnvelope.
    // This verifies root offset, vtable, and the nested version table in one call.
    ::flatbuffers::Verifier verifier(data, size);
    if (!::score::flatbuffers::VerifyBufferVersionEnvelopeBuffer(verifier))
    {
        return MakeUnexpected(ErrorCode::kVerificationFailed);
    }

    const auto* envelope = GetEnvelope(data);
    if (envelope == nullptr)
    {
        return MakeUnexpected(ErrorCode::kNullDataPointer);
    }

    const auto* version = envelope->version_info();
    if (version == nullptr)
    {
        return MakeUnexpected(ErrorCode::kVersionInfoNotPresent);
    }

    // Reject buffers where version_info was placed in the wrong vtable slot (user fbs).
    // The FlatBuffers verifier is type-blind and would otherwise accept another
    // field's bytes as a valid (all-default) BufferVersion table.
    // type_marker presence and correct value ensure no accidential match with a similar
    // table based field.
    const auto* marker = version->type_marker();
    if ((marker == nullptr) || (std::string_view{marker->c_str()} != "BV"))
    {
        return MakeUnexpected(ErrorCode::kVerificationFailed);
    }

    // Read the file_identifier from bytes 4-7
    char id[BufferVersionInfo::kIdentifierLength + 1U]{};
    std::memcpy(id, data + sizeof(::flatbuffers::uoffset_t), ::flatbuffers::kFileIdentifierLength);

    return BufferVersionInfo{id, version->major_version(), version->minor_version()};
}

inline score::Result<BufferVersionInfo> GetVersionImpl(const std::vector<uint8_t>& buf) noexcept
{
    if (buf.size() > 0)
    {
        return GetVersionImpl<>(buf.data(), buf.size());
    }
    else
    {
        // Avoid calling GetVersionImpl with a non-null pointer but zero size, which would fail verification.
        return MakeUnexpected(ErrorCode::kVerificationFailed);
    }
}

inline score::Result<void> VerifyVersionImpl(const uint8_t* data,
                                             const std::size_t size,
                                             const BufferVersionInfo& expected,
                                             const VersionMatchMode mode) noexcept
{
    const auto result = GetVersionImpl(data, size);
    if (!result.has_value())
    {
        return score::MakeUnexpected<void>(result.error());
    }

    const auto& actual = result.value();

    const bool match = (mode == VersionMatchMode::kMinorMinimum) ? ((actual.identifier() == expected.identifier()) &&
                                                                    (actual.major_version == expected.major_version) &&
                                                                    (actual.minor_version >= expected.minor_version))
                                                                 : (actual == expected);

    if (!match)
    {
        return MakeUnexpected(ErrorCode::kVersionMismatch);
    }

    return {};
}

inline score::Result<void> VerifyVersionImpl(const std::vector<uint8_t>& buf,
                                             const BufferVersionInfo& expected,
                                             const VersionMatchMode mode) noexcept
{
    if (buf.size() > 0)
    {
        return VerifyVersionImpl(buf.data(), buf.size(), expected, mode);
    }
    else
    {
        // Avoid calling VerifyVersion with a non-null pointer but zero size, which would fail verification.
        return MakeUnexpected(ErrorCode::kVerificationFailed);
    }
}

}  // namespace

// ─── VersionReader class ─────────────────────────────────────────────────────

score::Result<BufferVersionInfo> VersionReader::GetVersion(const uint8_t* data, const std::size_t size) noexcept
{
    return GetVersionImpl(data, size);
}

score::Result<BufferVersionInfo> VersionReader::GetVersion(const std::vector<uint8_t>& buf) noexcept
{
    return GetVersionImpl(buf);
}

score::Result<void> VersionReader::VerifyVersion(const uint8_t* data,
                                                 const std::size_t size,
                                                 const BufferVersionInfo& expected,
                                                 const VersionMatchMode mode) noexcept
{
    return VerifyVersionImpl(data, size, expected, mode);
}

score::Result<void> VersionReader::VerifyVersion(const std::vector<uint8_t>& buf,
                                                 const BufferVersionInfo& expected,
                                                 const VersionMatchMode mode) noexcept
{
    return VerifyVersionImpl(buf, expected, mode);
}

// ─── Convenience free functions ──────────────────────────────────────────────

score::Result<BufferVersionInfo> GetVersion(const uint8_t* data, const std::size_t size) noexcept
{
    return GetVersionImpl(data, size);
}

score::Result<BufferVersionInfo> GetVersion(const std::vector<uint8_t>& buf) noexcept
{
    return GetVersionImpl(buf);
}

score::Result<void> VerifyVersion(const uint8_t* data,
                                  const std::size_t size,
                                  const BufferVersionInfo& expected,
                                  const VersionMatchMode mode) noexcept
{
    return VerifyVersionImpl(data, size, expected, mode);
}

score::Result<void> VerifyVersion(const std::vector<uint8_t>& buf,
                                  const BufferVersionInfo& expected,
                                  const VersionMatchMode mode) noexcept
{
    return VerifyVersionImpl(buf, expected, mode);
}

}  // namespace flatbuffers
}  // namespace score
