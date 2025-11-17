#include "data_router_recorder.h"
#include "dlt/dlt_wrapper.hpp"
#include <cstdint>
#include <dlt/dlt.h>
#include <dlt/dlt_types.h>
#include <dlt/dlt_user.h>
#include <dlt/dlt_user_macros.h>
#include "score/mw/log/log_level.h"

namespace {

constexpr DltLogLevelType convert(const score::mw::log::LogLevel log_level) {
  return static_cast<DltLogLevelType>(log_level);
}

} // namespace

namespace score::mw::log::detail {

// Injectable constructor - accepts any IDlt implementation
DataRouterRecorder::DataRouterRecorder(
    const Configuration &config,
    score::cpp::pmr::memory_resource *memory_resource,
    std::unique_ptr<dlt::IDlt> dlt_implementation) noexcept
    : config_(config), memory_resource_(memory_resource),
      allocator_(memory_resource_),
      contextMap_(std::allocator_traits<decltype(allocator_)>::rebind_alloc<
                  decltype(contextMap_)::allocator_type>()),
      contextDataMap_(std::allocator_traits<decltype(allocator_)>::rebind_alloc<
                      decltype(contextDataMap_)::allocator_type>()),
      dlt_(std::move(dlt_implementation)) {

  static_cast<void>(dlt_->RegisterApp(config_.GetAppId().begin(), config_.GetAppDescription().begin()));
};

DataRouterRecorder::~DataRouterRecorder() noexcept {
  for (auto [index, context] : contextMap_) {
    static_cast<void>(dlt_->UnregisterContext(context));
  }

  static_cast<void>(dlt_->UnregisterApp());
}

score::cpp::optional<SlotHandle>
DataRouterRecorder::StartRecord(const std::string_view context_id,
                                const LogLevel log_level) noexcept {

  if (!IsLogEnabled(log_level, context_id)) {
    return score::cpp::nullopt;
  }

  LoggingIdentifier ctx_id{context_id};
  auto found_element = contextMap_.find(ctx_id);
  DltContext *context_to_log = nullptr;

  if (found_element == contextMap_.end()) {
    auto [inserted_element, insert_res] = contextMap_.try_emplace(ctx_id);
    if (!insert_res) {
      return score::cpp::nullopt;
    }

    auto register_context_res =
        dlt_->RegisterContext(
            inserted_element->second,
            inserted_element->first.GetStringView().begin(),
            "Context descriptions not supported");
    if(register_context_res < 0)
    {
      return score::cpp::nullopt;
    }

    context_to_log = &inserted_element->second;
  } else {
    context_to_log = &found_element->second;
  }

  DltContextData log_local;
  auto record_index =
      dlt_->StartLog(context_to_log, &log_local, convert(log_level));
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

  static_cast<void>(dlt_->FinishLog(&it.mapped()));
};

void DataRouterRecorder::Log(const SlotHandle &slot, const bool data) noexcept {
  static_cast<void>(LogImpl(slot, data));
};

void DataRouterRecorder::Log(const SlotHandle &slot, const std::uint8_t data) noexcept {
  static_cast<void>(LogImpl(slot, data));
};

void DataRouterRecorder::Log(const SlotHandle &slot, const std::int8_t data) noexcept {
  static_cast<void>(LogImpl(slot, data));
};

void DataRouterRecorder::Log(const SlotHandle &slot, const std::uint16_t data) noexcept {
  static_cast<void>(LogImpl(slot, data));
};

void DataRouterRecorder::Log(const SlotHandle &slot, const std::int16_t data) noexcept {
  static_cast<void>(LogImpl(slot, data));
};

void DataRouterRecorder::Log(const SlotHandle &slot, const std::uint32_t data) noexcept {
  static_cast<void>(LogImpl(slot, data));
};

void DataRouterRecorder::Log(const SlotHandle &slot, const std::int32_t data) noexcept {
  static_cast<void>(LogImpl(slot, data));
};

void DataRouterRecorder::Log(const SlotHandle &slot, const std::uint64_t data) noexcept {
  static_cast<void>(LogImpl(slot, data));
};

void DataRouterRecorder::Log(const SlotHandle &slot, const std::int64_t data) noexcept {
  static_cast<void>(LogImpl(slot, data));
};

void DataRouterRecorder::Log(const SlotHandle &slot, const float data) noexcept {
  static_cast<void>(LogImpl(slot, data));
};

void DataRouterRecorder::Log(const SlotHandle &slot, const double data) noexcept {
  static_cast<void>(LogImpl(slot, data));
};

void DataRouterRecorder::Log(const SlotHandle &slot, const std::string_view data) noexcept {
  static_cast<void>(LogImpl(slot, data));
};

void DataRouterRecorder::Log(const SlotHandle &slot, const LogHex8 data) noexcept {
  static_cast<void>(LogImpl(slot, data));
};

void DataRouterRecorder::Log(const SlotHandle &slot, const LogHex16 data) noexcept {
  static_cast<void>(LogImpl(slot, data));
};

void DataRouterRecorder::Log(const SlotHandle &slot, const LogHex32 data) noexcept {
  static_cast<void>(LogImpl(slot, data));
};

void DataRouterRecorder::Log(const SlotHandle &slot, const LogHex64 data) noexcept {
  static_cast<void>(LogImpl(slot, data));
};

void DataRouterRecorder::Log(const SlotHandle &slot, const LogBin8 data) noexcept {
  static_cast<void>(LogImpl(slot, data));
};

void DataRouterRecorder::Log(const SlotHandle &slot, const LogBin16 data) noexcept {
  static_cast<void>(LogImpl(slot, data));
};

void DataRouterRecorder::Log(const SlotHandle &slot, const LogBin32 data) noexcept {
  static_cast<void>(LogImplBin32(slot, data));
};

void DataRouterRecorder::Log(const SlotHandle &slot, const LogBin64 data) noexcept {
  static_cast<void>(LogImplBin64(slot, data));
};

void DataRouterRecorder::Log(const SlotHandle &slot, const LogRawBuffer data) noexcept {
  static_cast<void>(LogImpl(slot, data));
};

void DataRouterRecorder::Log(const SlotHandle &slot, const LogSlog2Message data) noexcept {
  static_cast<void>(LogImpl(slot, data));
};

// Special implementations for LogBin32 and LogBin64
DltReturnValue DataRouterRecorder::LogImplBin32(const SlotHandle &slot_handle, const LogBin32 &data) noexcept {
  auto it = contextDataMap_.find(static_cast<int>(slot_handle.GetSlotOfSelectedRecorder()));
  if (it == contextDataMap_.end()) {
    return DLT_RETURN_ERROR;
  }

  // LogBin32 needs to be split into two 16-bit values
  auto result1 = dlt_->LogUint16Formatted(&it->second, static_cast<std::uint16_t>((data.value >> 16U) & 0x0000FFFFU), DLT_FORMAT_BIN16);
  auto result2 = dlt_->LogUint16Formatted(&it->second, static_cast<std::uint16_t>(data.value & 0x0000FFFFU), DLT_FORMAT_BIN16);
  
  // Return the first error encountered, or OK if both succeed
  return (result1 < 0) ? result1 : result2;
}

DltReturnValue DataRouterRecorder::LogImplBin64(const SlotHandle &slot_handle, const LogBin64 &data) noexcept {
  auto it = contextDataMap_.find(static_cast<int>(slot_handle.GetSlotOfSelectedRecorder()));
  if (it == contextDataMap_.end()) {
    return DLT_RETURN_ERROR;
  }

  // LogBin64 needs to be split into four 16-bit values
  auto result1 = dlt_->LogUint16Formatted(&it->second, static_cast<std::uint16_t>((data.value >> 48U) & 0x000000000000FFFFU), DLT_FORMAT_BIN16);
  auto result2 = dlt_->LogUint16Formatted(&it->second, static_cast<std::uint16_t>((data.value >> 32U) & 0x000000000000FFFFU), DLT_FORMAT_BIN16);
  auto result3 = dlt_->LogUint16Formatted(&it->second, static_cast<std::uint16_t>((data.value >> 16U) & 0x000000000000FFFFU), DLT_FORMAT_BIN16);
  auto result4 = dlt_->LogUint16Formatted(&it->second, static_cast<std::uint16_t>(data.value & 0x000000000000FFFFU), DLT_FORMAT_BIN16);
  
  // Return the first error encountered, or OK if all succeed
  if (result1 < 0) return result1;
  if (result2 < 0) return result2;
  if (result3 < 0) return result3;
  return result4;
}

bool DataRouterRecorder::IsLogEnabled(
    const LogLevel &log_level, const std::string_view context) const noexcept {
  return config_.IsLogLevelEnabled(log_level, context, false);
};

} // namespace score::mw::log::detail
