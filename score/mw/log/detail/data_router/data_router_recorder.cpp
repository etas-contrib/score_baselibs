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
    : Recorder(), config_(config), ctx_{} {
  DLT_REGISTER_APP(config_.GetAppId().begin(),
                   config_.GetAppDescription().begin());
  DLT_REGISTER_CONTEXT(ctx_, "TEST", "XXX");
};

DataRouterRecorder::~DataRouterRecorder() noexcept {
  DLT_LOG(ctx_, DLT_LOG_ERROR, DLT_CSTRING("BYE"));

  for (auto [index, context] : contextMap_) {
    DLT_UNREGISTER_CONTEXT(context);
    std::cout << "unregistered " << index << "\n";
  }

  DLT_UNREGISTER_CONTEXT(ctx_);
  DLT_UNREGISTER_APP();
}

score::cpp::optional<SlotHandle>
DataRouterRecorder::StartRecord(const std::string_view context_id,
                                const LogLevel log_level) noexcept {

  std::string ctx_id{context_id};
  auto found_element = contextMap_.find(ctx_id);
  DltContext context;
  DltContext* context_to_log;
  std::cout << __FUNCTION__ << "\n";

  if (found_element == contextMap_.end()) {
    auto inserted_element = contextMap_.insert({ctx_id, context});
    if (!inserted_element.second) {
      std::cout << __FUNCTION__ << " Error inserting \n";
      return score::cpp::nullopt;
    }

    DLT_REGISTER_CONTEXT(inserted_element.first->second, // context
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

  auto res =
      contextDataMap_.insert_or_assign(static_cast<SlotIndex>(record_index), log_local);
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

  DltReturnValue res = dlt_user_log_write_finish(&it.mapped());
  if (static_cast<int>(res) < 0)
  {
    std::cout << __FUNCTION__ << " ERRRO?\n";
  }
  else{
    std::cout << __FUNCTION__ << " Written?\n";
  }
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
                             const std::string_view message) noexcept {
  LogImpl(slot, message.begin(), dlt_user_log_write_constant_string);
};

void DataRouterRecorder::Log(const SlotHandle &, const LogHex8) noexcept { };

void DataRouterRecorder::Log(const SlotHandle &, const LogHex16) noexcept {};

void DataRouterRecorder::Log(const SlotHandle &, const LogHex32) noexcept {};

void DataRouterRecorder::Log(const SlotHandle &, const LogHex64) noexcept {};

void DataRouterRecorder::Log(const SlotHandle &, const LogBin8) noexcept {};

void DataRouterRecorder::Log(const SlotHandle &, const LogBin16) noexcept {};

void DataRouterRecorder::Log(const SlotHandle &, const LogBin32) noexcept {};

void DataRouterRecorder::Log(const SlotHandle &, const LogBin64) noexcept {};

void DataRouterRecorder::Log(const SlotHandle &, const LogRawBuffer) noexcept {
};

void DataRouterRecorder::Log(const SlotHandle &,
                             const LogSlog2Message) noexcept {};

bool DataRouterRecorder::IsLogEnabled(
    const LogLevel &, const std::string_view context) const noexcept {
  return true;
};

} // namespace score::mw::log::detail
