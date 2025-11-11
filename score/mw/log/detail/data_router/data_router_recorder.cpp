#include "data_router_recorder.h"
#include <cstdint>
#include <dlt/dlt.h>
#include <dlt/dlt_types.h>
#include <dlt/dlt_user.h>
#include <dlt/dlt_user_macros.h>
#include <iostream>

namespace {

constexpr DltLogLevelType convert(const score::mw::log::LogLevel log_level) {
  return static_cast<DltLogLevelType>(log_level);
}

} // namespace

namespace score::mw::log::detail {

DataRouterRecorder::DataRouterRecorder(const Configuration &config) noexcept
    : Recorder(), config_(config) {
  DLT_REGISTER_APP(config_.GetAppId().begin(),
                   config_.GetAppDescription().begin());
};

DataRouterRecorder::~DataRouterRecorder() noexcept {
  for (auto [index, context] : contextMap_) {
    DLT_UNREGISTER_CONTEXT(context);
  }

  DLT_UNREGISTER_APP();
}

score::cpp::optional<SlotHandle>
DataRouterRecorder::StartRecord(const std::string_view context_id,
                                const LogLevel log_level) noexcept {

  if (!IsLogEnabled(log_level, context_id)) {
    return score::cpp::nullopt;
  }

  std::string ctx_id{context_id};
  auto found_element = contextMap_.find(ctx_id);
  DltContext *context_to_log;

  if (found_element == contextMap_.end()) {
    auto inserted_element = contextMap_.insert({ctx_id, DltContext{}});
    if (!inserted_element.second) {
      return score::cpp::nullopt;
    }

    DLT_REGISTER_CONTEXT(inserted_element.first->second,        // context
                         inserted_element.first->first.c_str(), // context id
                         "XXX");
    context_to_log = &inserted_element.first->second;
  } else {
    context_to_log = &found_element->second;
  }

  log_level_ = convert(log_level);
  DltContextData log_local;
  auto record_index =
      dlt_user_log_write_start(context_to_log, &log_local, log_level_);
  if (record_index < 0) {
    return score::cpp::nullopt;
  }

  auto res = contextDataMap_.insert_or_assign(
      static_cast<SlotIndex>(record_index), log_local);
  if (res.second) {
    return SlotHandle(static_cast<SlotIndex>(record_index));
  } else {
    return score::cpp::nullopt;
  }
};

void DataRouterRecorder::StopRecord(const SlotHandle &slot) noexcept {
  auto it = contextDataMap_.extract(slot.GetSlotOfSelectedRecorder());

  if (it.empty()) {
    return;
  }

  static_cast<void>(dlt_user_log_write_finish(&it.mapped()));
};

void DataRouterRecorder::Log(const SlotHandle &slot, const bool data) noexcept {
  LogImpl(slot, data, dlt_user_log_write_bool);
};

void DataRouterRecorder::Log(const SlotHandle &slot,
                             const std::uint8_t data) noexcept {
  LogImpl(slot, data, dlt_user_log_write_uint8);
};
void DataRouterRecorder::Log(const SlotHandle &slot,
                             const std::int8_t data) noexcept {
  LogImpl(slot, data, dlt_user_log_write_int8);
};

void DataRouterRecorder::Log(const SlotHandle &slot,
                             const std::uint16_t data) noexcept {
  LogImpl(slot, data, dlt_user_log_write_uint16);
};

void DataRouterRecorder::Log(const SlotHandle &slot,
                             const std::int16_t data) noexcept {
  LogImpl(slot, data, dlt_user_log_write_int16);
};

void DataRouterRecorder::Log(const SlotHandle &slot,
                             const std::uint32_t data) noexcept {
  LogImpl(slot, data, dlt_user_log_write_uint32);
};
void DataRouterRecorder::Log(const SlotHandle &slot,
                             const std::int32_t data) noexcept {
  LogImpl(slot, data, dlt_user_log_write_uint32);
};

void DataRouterRecorder::Log(const SlotHandle &slot,
                             const std::uint64_t data) noexcept {
  LogImpl(slot, data, dlt_user_log_write_uint64);
};

void DataRouterRecorder::Log(const SlotHandle &slot,
                             const std::int64_t data) noexcept {
  LogImpl(slot, data, dlt_user_log_write_int64);
};

void DataRouterRecorder::Log(const SlotHandle &slot,
                             const float data) noexcept {
  LogImpl(slot, data, dlt_user_log_write_float32);
};
void DataRouterRecorder::Log(const SlotHandle &slot,
                             const double data) noexcept {

  LogImpl(slot, data, dlt_user_log_write_float64);
};

void DataRouterRecorder::Log(const SlotHandle &slot,
                             const std::string_view data) noexcept {
  LogImpl(slot, data.begin(), dlt_user_log_write_constant_string);
};

void DataRouterRecorder::Log(const SlotHandle &slot,
                             const LogHex8 data) noexcept {
  auto log_hex = [](DltContextData *ctx, const LogHex8 data) {
    dlt_user_log_write_uint8_formatted(ctx, data.value, DLT_FORMAT_HEX8);
  };
  LogImpl(slot, data, log_hex);
};

void DataRouterRecorder::Log(const SlotHandle &slot,
                             const LogHex16 data) noexcept {
  auto log_hex = [](DltContextData *ctx, const LogHex16 data) {
    dlt_user_log_write_uint16_formatted(ctx, data.value, DLT_FORMAT_HEX16);
  };
  LogImpl(slot, data, log_hex);
};

void DataRouterRecorder::Log(const SlotHandle &slot,
                             const LogHex32 data) noexcept {
  auto log_hex = [](DltContextData *ctx, const LogHex32 data) {
    dlt_user_log_write_uint32_formatted(ctx, data.value, DLT_FORMAT_HEX32);
  };
  LogImpl(slot, data, log_hex);
};

void DataRouterRecorder::Log(const SlotHandle &slot,
                             const LogHex64 data) noexcept {
  auto log_hex = [](DltContextData *ctx, const LogHex64 data) {
    dlt_user_log_write_uint64_formatted(ctx, data.value, DLT_FORMAT_HEX64);
  };
  LogImpl(slot, data, log_hex);
};

void DataRouterRecorder::Log(const SlotHandle &slot,
                             const LogBin8 data) noexcept {
  auto log_bin = [](DltContextData *ctx, const LogBin8 data) {
    dlt_user_log_write_uint8_formatted(ctx, data.value, DLT_FORMAT_BIN8);
  };
  LogImpl(slot, data, log_bin);
};

void DataRouterRecorder::Log(const SlotHandle &slot,
                             const LogBin16 data) noexcept {
  auto log_bin = [](DltContextData *ctx, const LogBin16 data) {
    dlt_user_log_write_uint16_formatted(ctx, data.value, DLT_FORMAT_BIN16);
  };
  LogImpl(slot, data, log_bin);
};

void DataRouterRecorder::Log(const SlotHandle &slot,
                             const LogBin32 data) noexcept {
  // auto log_bin = [](DltContextData* ctx, const std::uint16_t data)
  // {
  //     dlt_user_log_write_uint16_formatted(ctx, data, DLT_FORMAT_BIN16);
  // };
  // LogImpl(slot, data.value >> 16, log_bin);
  // LogImpl(slot, data.value & 0xFFFF, log_bin);
};

void DataRouterRecorder::Log(const SlotHandle &slot,
                             const LogBin64 data) noexcept {
  // auto log_bin = [](DltContextData* ctx, const LogHex8 value)
  // {
  //     dlt_user_log_write_uint8_formatted(ctx, value.value, DLT_FORMAT_HEX64);
  // };
  // LogImpl(slot, data, log_bin);
};

void DataRouterRecorder::Log(const SlotHandle &slot, const LogRawBuffer data) noexcept {
  LogImpl(slot, data.data(), dlt_user_log_write_constant_string);
};

void DataRouterRecorder::Log(const SlotHandle &slot,
                             const LogSlog2Message data) noexcept {

  LogImpl(slot, data.GetMessage().begin(), dlt_user_log_write_constant_string);
};

bool DataRouterRecorder::IsLogEnabled(
    const LogLevel & log_level, const std::string_view context) const noexcept {
    // return true;
    return config_.IsLogLevelEnabled(log_level, context, false);
};

} // namespace score::mw::log::detail
