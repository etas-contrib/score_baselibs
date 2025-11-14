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

DataRouterRecorder::DataRouterRecorder(
    const Configuration &config,
    score::cpp::pmr::memory_resource *memory_resource) noexcept
    : DataRouterRecorder(config, memory_resource, std::make_unique<dlt::Dlt>()) {
}

// Injectable constructor - accepts any IDlt implementation
DataRouterRecorder::DataRouterRecorder(
    const Configuration &config,
    score::cpp::pmr::memory_resource *memory_resource,
    std::unique_ptr<dlt::IDlt> dlt_implementation) noexcept
    : Recorder(), config_(config), memory_resource_(memory_resource),
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
  DltContext *context_to_log;

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
  auto log_bool = [&dlt_ = dlt_](DltContextData *ctx, const bool value) -> DltReturnValue {
    return dlt_->LogBool(ctx, value);
  };
  static_cast<void>(LogImpl(slot, data, log_bool));
};

void DataRouterRecorder::Log(const SlotHandle &slot,
                             const std::uint8_t data) noexcept {
  auto log_uint8 = [&dlt_ = dlt_](DltContextData *ctx, const std::uint8_t value) -> DltReturnValue {
    return dlt_->LogUint8(ctx, value);
  };
  static_cast<void>(LogImpl(slot, data, log_uint8));
};
void DataRouterRecorder::Log(const SlotHandle &slot,
                             const std::int8_t data) noexcept {
  auto log_int8 = [&dlt_ = dlt_](DltContextData *ctx, const std::int8_t value) -> DltReturnValue {
    return dlt_->LogInt8(ctx, value);
  };
  static_cast<void>(LogImpl(slot, data, log_int8));
};

void DataRouterRecorder::Log(const SlotHandle &slot,
                             const std::uint16_t data) noexcept {
  auto log_uint16 = [&dlt_ = dlt_](DltContextData *ctx, const std::uint16_t value) -> DltReturnValue {
    return dlt_->LogUint16(ctx, value);
  };
  static_cast<void>(LogImpl(slot, data, log_uint16));
};

void DataRouterRecorder::Log(const SlotHandle &slot,
                             const std::int16_t data) noexcept {
  auto log_int16 = [&dlt_ = dlt_](DltContextData *ctx, const std::int16_t value) -> DltReturnValue {
    return dlt_->LogInt16(ctx, value);
  };
  static_cast<void>(LogImpl(slot, data, log_int16));
};

void DataRouterRecorder::Log(const SlotHandle &slot,
                             const std::uint32_t data) noexcept {
  auto log_uint32 = [&dlt_ = dlt_](DltContextData *ctx, const std::uint32_t value) -> DltReturnValue {
    return dlt_->LogUint32(ctx, value);
  };
  static_cast<void>(LogImpl(slot, data, log_uint32));
};
void DataRouterRecorder::Log(const SlotHandle &slot,
                             const std::int32_t data) noexcept {
  auto log_int32 = [&dlt_ = dlt_](DltContextData *ctx, const std::int32_t value) -> DltReturnValue {
    return dlt_->LogInt32(ctx, value);
  };
  static_cast<void>(LogImpl(slot, data, log_int32));
};

void DataRouterRecorder::Log(const SlotHandle &slot,
                             const std::uint64_t data) noexcept {
  auto log_uint64 = [&dlt_ = dlt_](DltContextData *ctx, const std::uint64_t value) -> DltReturnValue {
    return dlt_->LogUint64(ctx, value);
  };
  static_cast<void>(LogImpl(slot, data, log_uint64));
};

void DataRouterRecorder::Log(const SlotHandle &slot,
                             const std::int64_t data) noexcept {
  auto log_int64 = [&dlt_ = dlt_](DltContextData *ctx, const std::int64_t value) -> DltReturnValue {
    return dlt_->LogInt64(ctx, value);
  };
  static_cast<void>(LogImpl(slot, data, log_int64));
};

void DataRouterRecorder::Log(const SlotHandle &slot,
                             const float data) noexcept {
  auto log_float32 = [&dlt_ = dlt_](DltContextData *ctx, const float value) -> DltReturnValue {
    return dlt_->LogFloat32(ctx, value);
  };
  static_cast<void>(LogImpl(slot, data, log_float32));
};
void DataRouterRecorder::Log(const SlotHandle &slot,
                             const double data) noexcept {
  auto log_float64 = [&dlt_ = dlt_](DltContextData *ctx, const double value) -> DltReturnValue {
    return dlt_->LogFloat64(ctx, value);
  };
  static_cast<void>(LogImpl(slot, data, log_float64));
};

void DataRouterRecorder::Log(const SlotHandle &slot,
                             const std::string_view data) noexcept {
  auto log_string = [&dlt_ = dlt_](DltContextData *ctx, const std::string_view value) -> DltReturnValue {
    return dlt_->LogString(ctx, value.data());
  };
  static_cast<void>(LogImpl(slot, data, log_string));
};

void DataRouterRecorder::Log(const SlotHandle &slot,
                             const LogHex8 data) noexcept {
  auto log_hex = [&dlt_ = dlt_](DltContextData *ctx, const LogHex8 value) -> DltReturnValue {
    return dlt_->LogUint8Formatted(ctx, value.value, DLT_FORMAT_HEX8);
  };
  static_cast<void>(LogImpl(slot, data, log_hex));
};

void DataRouterRecorder::Log(const SlotHandle &slot,
                             const LogHex16 data) noexcept {
  auto log_hex = [&dlt_ = dlt_](DltContextData *ctx, const LogHex16 value) -> DltReturnValue {
    return dlt_->LogUint16Formatted(ctx, value.value, DLT_FORMAT_HEX16);
  };
  static_cast<void>(LogImpl(slot, data, log_hex));
};

void DataRouterRecorder::Log(const SlotHandle &slot,
                             const LogHex32 data) noexcept {
  auto log_hex = [&dlt_ = dlt_](DltContextData *ctx, const LogHex32 value) -> DltReturnValue {
    return dlt_->LogUint32Formatted(ctx, value.value, DLT_FORMAT_HEX32);
  };
  static_cast<void>(LogImpl(slot, data, log_hex));
};

void DataRouterRecorder::Log(const SlotHandle &slot,
                             const LogHex64 data) noexcept {
  auto log_hex = [&dlt_ = dlt_](DltContextData *ctx, const LogHex64 value) -> DltReturnValue {
    return dlt_->LogUint64Formatted(ctx, value.value, DLT_FORMAT_HEX64);
  };
  static_cast<void>(LogImpl(slot, data, log_hex));
};

void DataRouterRecorder::Log(const SlotHandle &slot,
                             const LogBin8 data) noexcept {
  auto log_bin = [&dlt_ = dlt_](DltContextData *ctx, const LogBin8 value) -> DltReturnValue {
    return dlt_->LogUint8Formatted(ctx, value.value, DLT_FORMAT_BIN8);
  };
  static_cast<void>(LogImpl(slot, data, log_bin));
};

void DataRouterRecorder::Log(const SlotHandle &slot,
                             const LogBin16 data) noexcept {
  auto log_bin = [&dlt_ = dlt_](DltContextData *ctx, const LogBin16 value) -> DltReturnValue {
    return dlt_->LogUint16Formatted(ctx, value.value, DLT_FORMAT_BIN16);
  };
  static_cast<void>(LogImpl(slot, data, log_bin));
};

void DataRouterRecorder::Log(const SlotHandle &slot,
                             const LogBin32 data) noexcept {
  auto log_bin = [&dlt_ = dlt_](DltContextData *ctx, const std::uint16_t value) -> DltReturnValue {
    return dlt_->LogUint16Formatted(ctx, value, DLT_FORMAT_BIN16);
  };

  static_cast<void>(LogImpl(slot, static_cast<std::uint16_t>((data.value >> 16U) & 0x0000FFFFU), log_bin));
  static_cast<void>(LogImpl(slot, static_cast<std::uint16_t>(data.value & 0x0000FFFFU), log_bin));
};

void DataRouterRecorder::Log(const SlotHandle &slot,
                             const LogBin64 data) noexcept {
  auto log_bin = [&dlt_ = dlt_](DltContextData *ctx, const std::uint16_t value) -> DltReturnValue {
    return dlt_->LogUint16Formatted(ctx, value, DLT_FORMAT_BIN16);
  };
  static_cast<void>(LogImpl(slot, static_cast<std::uint16_t>((data.value >> 48U) & 0x000000000000FFFFU), log_bin));
  static_cast<void>(LogImpl(slot, static_cast<std::uint16_t>((data.value >> 32U) & 0x000000000000FFFFU), log_bin));
  static_cast<void>(LogImpl(slot, static_cast<std::uint16_t>((data.value >> 16U) & 0x000000000000FFFFU), log_bin));
  static_cast<void>(LogImpl(slot, static_cast<std::uint16_t>(data.value & 0x000000000000FFFFU), log_bin));
};

void DataRouterRecorder::Log(const SlotHandle &slot,
                             const LogRawBuffer data) noexcept {
  auto log_string = [&dlt_ = dlt_](DltContextData *ctx, const char* value) -> DltReturnValue {
    return dlt_->LogString(ctx, value);
  };
  static_cast<void>(LogImpl(slot, data.data(), log_string));
};

void DataRouterRecorder::Log(const SlotHandle &slot,
                             const LogSlog2Message data) noexcept {
  auto log_string = [&dlt_ = dlt_](DltContextData *ctx, const char* value) -> DltReturnValue {
    return dlt_->LogString(ctx, value);
  };
  static_cast<void>(LogImpl(slot, data.GetMessage().data(), log_string));
};

bool DataRouterRecorder::IsLogEnabled(
    const LogLevel &log_level, const std::string_view context) const noexcept {
  return config_.IsLogLevelEnabled(log_level, context, false);
};

} // namespace score::mw::log::detail
