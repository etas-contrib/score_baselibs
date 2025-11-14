
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

#include "dlt_wrapper.hpp"
#include <dlt/dlt_user.h>
#include <string>

namespace score::mw::log::detail::dlt {

DltReturnValue Dlt::RegisterApp(std::string_view app_id,
                                std::string_view description) const noexcept {
  return dlt_register_app(app_id.begin(), description.begin());
}

DltReturnValue Dlt::UnregisterApp() const noexcept {
  return dlt_unregister_app();
}

DltReturnValue Dlt::RegisterContext(DltContext &context, 
                                   std::string_view context_id,
                                   std::string_view description) noexcept {
  const std::string context_id_str{context_id};
  const std::string description_str{description};
  return dlt_register_context(&context, context_id_str.c_str(), description_str.c_str());
}

DltReturnValue Dlt::UnregisterContext(DltContext &context) noexcept {
  return dlt_unregister_context(&context);
}

DltReturnValue Dlt::StartLog(DltContext *context,
                            DltContextData *record,
                            DltLogLevelType log_level) noexcept {
  return dlt_user_log_write_start(context, record, log_level);
}

DltReturnValue Dlt::FinishLog(DltContextData *record) noexcept {
  return dlt_user_log_write_finish(record);
}

DltReturnValue Dlt::LogBool(DltContextData *record, bool data) noexcept {
  return dlt_user_log_write_bool(record, static_cast<std::uint8_t>(data));
}

DltReturnValue Dlt::LogUint8(DltContextData *record, std::uint8_t data) noexcept {
  return dlt_user_log_write_uint8(record, data);
}

DltReturnValue Dlt::LogInt8(DltContextData *record, std::int8_t data) noexcept {
  return dlt_user_log_write_int8(record, data);
}

DltReturnValue Dlt::LogUint16(DltContextData *record, std::uint16_t data) noexcept {
  return dlt_user_log_write_uint16(record, data);
}

DltReturnValue Dlt::LogInt16(DltContextData *record, std::int16_t data) noexcept {
  return dlt_user_log_write_int16(record, data);
}

DltReturnValue Dlt::LogUint32(DltContextData *record, std::uint32_t data) noexcept {
  return dlt_user_log_write_uint32(record, data);
}

DltReturnValue Dlt::LogInt32(DltContextData *record, std::int32_t data) noexcept {
  return dlt_user_log_write_int32(record, data);
}

DltReturnValue Dlt::LogUint64(DltContextData *record, std::uint64_t data) noexcept {
  return dlt_user_log_write_uint64(record, data);
}

DltReturnValue Dlt::LogInt64(DltContextData *record, std::int64_t data) noexcept {
  return dlt_user_log_write_int64(record, data);
}

DltReturnValue Dlt::LogFloat32(DltContextData *record, float data) noexcept {
  return dlt_user_log_write_float32(record, data);
}

DltReturnValue Dlt::LogFloat64(DltContextData *record, double data) noexcept {
  return dlt_user_log_write_float64(record, data);
}

DltReturnValue Dlt::LogString(DltContextData *record, const char* data) noexcept {
  return dlt_user_log_write_constant_string(record, data);
}

DltReturnValue Dlt::LogUint8Formatted(DltContextData *record, std::uint8_t data, DltFormatType format) noexcept {
  return dlt_user_log_write_uint8_formatted(record, data, format);
}

DltReturnValue Dlt::LogUint16Formatted(DltContextData *record, std::uint16_t data, DltFormatType format) noexcept {
  return dlt_user_log_write_uint16_formatted(record, data, format);
}

DltReturnValue Dlt::LogUint32Formatted(DltContextData *record, std::uint32_t data, DltFormatType format) noexcept {
  return dlt_user_log_write_uint32_formatted(record, data, format);
}

DltReturnValue Dlt::LogUint64Formatted(DltContextData *record, std::uint64_t data, DltFormatType format) noexcept {
  return dlt_user_log_write_uint64_formatted(record, data, format);
}

DltReturnValue Dlt::LogArithmatic(DltContextData &record,
                                 std::uint16_t data) noexcept {
  return dlt_user_log_write_uint16(&record, data);
}

}
