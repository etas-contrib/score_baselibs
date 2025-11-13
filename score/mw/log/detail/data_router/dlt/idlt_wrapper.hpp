/********************************************************************************
 * Copyright (c) 2025 Contributors to the Eclipse Foundation
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
#ifndef SCORE_MW_LOG_DETAIL_DATA_ROUTER_IDLT_WRAPPER_H
#define SCORE_MW_LOG_DETAIL_DATA_ROUTER_IDLT_WRAPPER_H

#include <cstdint>
#include <string_view>
#include <dlt/dlt_types.h>
#include <dlt/dlt_user.h>

namespace score::mw::log::detail::dlt {

class IDlt {
public:
  IDlt() noexcept = default;
  virtual ~IDlt() = default;

  IDlt(const IDlt &) = delete;
  IDlt &operator=(const IDlt &) = delete;
  IDlt(IDlt &&) = delete;
  IDlt &operator=(IDlt &&) = delete;

  [[nodiscard]] virtual DltReturnValue
  RegisterApp(std::string_view app_id,
              std::string_view description) const noexcept = 0;

  [[nodiscard]] virtual DltReturnValue
  UnregisterApp() const noexcept = 0;

  [[nodiscard]] virtual DltReturnValue
  RegisterContext(DltContext &context, std::string_view context_id,
                  std::string_view description) noexcept = 0;

  [[nodiscard]] virtual DltReturnValue
  UnregisterContext(DltContext &context) noexcept = 0;

  [[nodiscard]] virtual DltReturnValue
  StartLog(DltContext* context, DltContextData* record,
           DltLogLevelType log_level) noexcept = 0;

  [[nodiscard]] virtual DltReturnValue
  FinishLog(DltContextData *record) noexcept = 0;

  [[nodiscard]] virtual DltReturnValue
  LogBool(DltContextData *record, bool data) noexcept = 0;

  [[nodiscard]] virtual DltReturnValue
  LogUint8(DltContextData *record, std::uint8_t data) noexcept = 0;

  [[nodiscard]] virtual DltReturnValue
  LogInt8(DltContextData *record, std::int8_t data) noexcept = 0;

  [[nodiscard]] virtual DltReturnValue
  LogUint16(DltContextData *record, std::uint16_t data) noexcept = 0;

  [[nodiscard]] virtual DltReturnValue
  LogInt16(DltContextData *record, std::int16_t data) noexcept = 0;

  [[nodiscard]] virtual DltReturnValue
  LogUint32(DltContextData *record, std::uint32_t data) noexcept = 0;

  [[nodiscard]] virtual DltReturnValue
  LogInt32(DltContextData *record, std::int32_t data) noexcept = 0;

  [[nodiscard]] virtual DltReturnValue
  LogUint64(DltContextData *record, std::uint64_t data) noexcept = 0;

  [[nodiscard]] virtual DltReturnValue
  LogInt64(DltContextData *record, std::int64_t data) noexcept = 0;

  [[nodiscard]] virtual DltReturnValue
  LogFloat32(DltContextData *record, float data) noexcept = 0;

  [[nodiscard]] virtual DltReturnValue
  LogFloat64(DltContextData *record, double data) noexcept = 0;

  [[nodiscard]] virtual DltReturnValue
  LogString(DltContextData *record, const char* data) noexcept = 0;

  [[nodiscard]] virtual DltReturnValue
  LogUint8Formatted(DltContextData *record, std::uint8_t data, DltFormatType format) noexcept = 0;

  [[nodiscard]] virtual DltReturnValue
  LogUint16Formatted(DltContextData *record, std::uint16_t data, DltFormatType format) noexcept = 0;

  [[nodiscard]] virtual DltReturnValue
  LogUint32Formatted(DltContextData *record, std::uint32_t data, DltFormatType format) noexcept = 0;

  [[nodiscard]] virtual DltReturnValue
  LogUint64Formatted(DltContextData *record, std::uint64_t data, DltFormatType format) noexcept = 0;

  [[nodiscard]] virtual DltReturnValue
  LogArithmatic(DltContextData &record, std::uint16_t data) noexcept = 0;
};

} // namespace score::mw::log::detail::dlt
#endif // SCORE_MW_LOG_DETAIL_DATA_ROUTER_IDLT_WRAPPER_H
