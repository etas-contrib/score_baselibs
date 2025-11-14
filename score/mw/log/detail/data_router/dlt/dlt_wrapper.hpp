
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
#ifndef SCORE_MW_LOG_DETAIL_DATA_ROUTER_DLT_WRAPPER_H
#define SCORE_MW_LOG_DETAIL_DATA_ROUTER_DLT_WRAPPER_H

#include "idlt_wrapper.hpp"

namespace score::mw::log::detail::dlt {

class Dlt : public IDlt {
public:
  Dlt() noexcept = default;
  ~Dlt() override = default;

  Dlt(const Dlt&) = delete;
  Dlt& operator=(const Dlt&) = delete;
  Dlt(Dlt&&) = delete;
  Dlt& operator=(Dlt&&) = delete;

  [[nodiscard]] DltReturnValue RegisterApp(std::string_view app_id,
                                           std::string_view description) const noexcept override;

  [[nodiscard]] DltReturnValue UnregisterApp() const noexcept override;

  [[nodiscard]] DltReturnValue RegisterContext(DltContext &context, 
                                     std::string_view context_id,
                                     std::string_view description) noexcept override;

  [[nodiscard]] DltReturnValue UnregisterContext(DltContext &context) noexcept override;

  [[nodiscard]] DltReturnValue StartLog(DltContext* context,
                              DltContextData* record,
                              DltLogLevelType log_level) noexcept override;

  [[nodiscard]] DltReturnValue FinishLog(DltContextData *record) noexcept override;

  [[nodiscard]] DltReturnValue LogBool(DltContextData *record, bool data) noexcept override;
  [[nodiscard]] DltReturnValue LogUint8(DltContextData *record, std::uint8_t data) noexcept override;
  [[nodiscard]] DltReturnValue LogInt8(DltContextData *record, std::int8_t data) noexcept override;
  [[nodiscard]] DltReturnValue LogUint16(DltContextData *record, std::uint16_t data) noexcept override;
  [[nodiscard]] DltReturnValue LogInt16(DltContextData *record, std::int16_t data) noexcept override;
  [[nodiscard]] DltReturnValue LogUint32(DltContextData *record, std::uint32_t data) noexcept override;
  [[nodiscard]] DltReturnValue LogInt32(DltContextData *record, std::int32_t data) noexcept override;
  [[nodiscard]] DltReturnValue LogUint64(DltContextData *record, std::uint64_t data) noexcept override;
  [[nodiscard]] DltReturnValue LogInt64(DltContextData *record, std::int64_t data) noexcept override;
  [[nodiscard]] DltReturnValue LogFloat32(DltContextData *record, float data) noexcept override;
  [[nodiscard]] DltReturnValue LogFloat64(DltContextData *record, double data) noexcept override;
  [[nodiscard]] DltReturnValue LogString(DltContextData *record, const char* data) noexcept override;
  [[nodiscard]] DltReturnValue LogUint8Formatted(DltContextData *record, std::uint8_t data, DltFormatType format) noexcept override;
  [[nodiscard]] DltReturnValue LogUint16Formatted(DltContextData *record, std::uint16_t data, DltFormatType format) noexcept override;
  [[nodiscard]] DltReturnValue LogUint32Formatted(DltContextData *record, std::uint32_t data, DltFormatType format) noexcept override;
  [[nodiscard]] DltReturnValue LogUint64Formatted(DltContextData *record, std::uint64_t data, DltFormatType format) noexcept override;

  [[nodiscard]] DltReturnValue LogArithmatic(DltContextData &record,
                                   std::uint16_t data) noexcept override;
};

} // namespace score::mw::log::detail::dlt

#endif // SCORE_MW_LOG_DETAIL_DATA_ROUTER_DLT_WRAPPER_H
