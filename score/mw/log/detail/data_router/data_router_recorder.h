#ifndef SCORE_MW_LOG_DLT_RECORDER_H_
#define SCORE_MW_LOG_DLT_RECORDER_H_

#include <memory>
#include <unordered_map>
#include <cstdint>

#include <score/memory.hpp>
#include <score/optional.hpp>

#include "score/mw/log/configuration/configuration.h"
#include "score/mw/log/recorder.h"
#include "score/mw/log/slot_handle.h"
#include "score/mw/log/detail/data_router/dlt/idlt_wrapper.hpp"
#include <dlt/dlt_types.h>
#include <dlt/dlt_user.h>


namespace score::mw::log::detail {

class DataRouterRecorder : public Recorder {
public:
//   // Constructor that accepts a DLT implementation - for production use
//   DataRouterRecorder(
//       const Configuration &config,
//       score::cpp::pmr::memory_resource *memory_resource) noexcept;

  // Constructor that accepts a custom DLT implementation - for testing/mocking
  DataRouterRecorder(
      const Configuration &config,
      score::cpp::pmr::memory_resource *memory_resource,
      std::unique_ptr<dlt::IDlt> dlt_implementation) noexcept;

  ~DataRouterRecorder() noexcept override;

  score::cpp::optional<SlotHandle>
  StartRecord(const std::string_view context_id,
              const LogLevel log_level) noexcept override;

  void StopRecord(const SlotHandle &slot) noexcept override;

  void Log(const SlotHandle &slot, const bool data) noexcept override;

  void Log(const SlotHandle &slot, const std::uint8_t data) noexcept override;
  void Log(const SlotHandle &slot, const std::int8_t data) noexcept override;

  void Log(const SlotHandle &slot, const std::uint16_t data) noexcept override;
  void Log(const SlotHandle &slot, const std::int16_t data) noexcept override;

  void Log(const SlotHandle &slot, const std::uint32_t data) noexcept override;
  void Log(const SlotHandle &slot, const std::int32_t data) noexcept override;

  void Log(const SlotHandle &slot, const std::uint64_t data) noexcept override;
  void Log(const SlotHandle &slot, const std::int64_t data) noexcept override;

  void Log(const SlotHandle &slot, const float data) noexcept override;
  void Log(const SlotHandle &slot, const double data) noexcept override;

  void Log(const SlotHandle &slot, const std::string_view data) noexcept override;

  void Log(const SlotHandle &slot, const LogHex8 data) noexcept override;

  void Log(const SlotHandle &slot, const LogHex16 data) noexcept override;

  void Log(const SlotHandle &slot, const LogHex32 data) noexcept override;

  void Log(const SlotHandle &slot, const LogHex64 data) noexcept override;

  void Log(const SlotHandle &slot, const LogBin8 data) noexcept override;

  void Log(const SlotHandle &slot, const LogBin16 data) noexcept override;

  void Log(const SlotHandle &slot, const LogBin32 data) noexcept override;

  void Log(const SlotHandle &slot, const LogBin64 data) noexcept override;

  void Log(const SlotHandle &slot, const LogRawBuffer data) noexcept override;

  void Log(const SlotHandle &slot, const LogSlog2Message data) noexcept override;

  bool IsLogEnabled(const LogLevel &log_level,
                    const std::string_view context) const noexcept override;

private:
  template <typename T>
  DltReturnValue
  LogImpl(const SlotHandle &slot_handle, const T &data)
       noexcept {

    auto found_element = contextDataMap_.find(
        static_cast<int>(slot_handle.GetSlotOfSelectedRecorder()));
    if (found_element == contextDataMap_.end()) {
        return DLT_RETURN_ERROR;
    }

    if constexpr (std::is_same_v<T, bool>) {
        return dlt_->LogBool(&found_element->second, data);
    }
    else if constexpr (std::is_same_v<T, std::uint8_t>) {
        return dlt_->LogUint8(&found_element->second, data);
    }
    else if constexpr (std::is_same_v<T, std::int8_t>) {
        return dlt_->LogInt8(&found_element->second, data);
    }
    else if constexpr (std::is_same_v<T, std::uint16_t>) {
        return dlt_->LogUint16(&found_element->second, data);
    }
    else if constexpr (std::is_same_v<T, std::int16_t>) {
        return dlt_->LogInt16(&found_element->second, data);
    }
    else if constexpr (std::is_same_v<T, std::uint32_t>) {
        return dlt_->LogUint32(&found_element->second, data);
    }
    else if constexpr (std::is_same_v<T, std::int32_t>) {
        return dlt_->LogInt32(&found_element->second, data);
    }
    else if constexpr (std::is_same_v<T, std::uint64_t>) {
        return dlt_->LogUint64(&found_element->second, data);
    }
    else if constexpr (std::is_same_v<T, std::int64_t>) {
        return dlt_->LogInt64(&found_element->second, data);
    }
    else if constexpr (std::is_same_v<T, float>) {
        return dlt_->LogFloat32(&found_element->second, data);
    }
    else if constexpr (std::is_same_v<T, double>) {
        return dlt_->LogFloat64(&found_element->second, data);
    }
    else if constexpr (std::is_same_v<T, std::string_view>) {
        return dlt_->LogString(&found_element->second, data.data());
    }
    else if constexpr (std::is_same_v<T, LogHex8>) {
        return dlt_->LogUint8Formatted(&found_element->second, data.value, DLT_FORMAT_HEX8);
    }
    else if constexpr (std::is_same_v<T, LogHex16>) {
        return dlt_->LogUint16Formatted(&found_element->second, data.value, DLT_FORMAT_HEX16);
    }
    else if constexpr (std::is_same_v<T, LogHex32>) {
        return dlt_->LogUint32Formatted(&found_element->second, data.value, DLT_FORMAT_HEX32);
    }
    else if constexpr (std::is_same_v<T, LogHex64>) {
        return dlt_->LogUint64Formatted(&found_element->second, data.value, DLT_FORMAT_HEX64);
    }
    else if constexpr (std::is_same_v<T, LogBin8>) {
        return dlt_->LogUint8Formatted(&found_element->second, data.value, DLT_FORMAT_BIN8);
    }
    else if constexpr (std::is_same_v<T, LogBin16>) {
        return dlt_->LogUint16Formatted(&found_element->second, data.value, DLT_FORMAT_BIN16);
    }
    else if constexpr (std::is_same_v<T, LogRawBuffer>) {
        return dlt_->LogString(&found_element->second, data.data());
    }
    else if constexpr (std::is_same_v<T, LogSlog2Message>) {
        return dlt_->LogString(&found_element->second, data.GetMessage().data());
    }
    else {
        static_assert(sizeof(T) == 0, "Unsupported type for LogImpl");
        return DLT_RETURN_ERROR;
    }
  }

  // Special handling for LogBin32 and LogBin64 which need multiple DLT calls
  DltReturnValue LogImplBin32(const SlotHandle &slot_handle, const LogBin32 &data) noexcept;
  DltReturnValue LogImplBin64(const SlotHandle &slot_handle, const LogBin64 &data) noexcept;

  Configuration config_;

  score::cpp::pmr::memory_resource *memory_resource_;

  score::cpp::pmr::polymorphic_allocator<void> allocator_;

  /// \brief A Map of all open `DLTContexts`.
  std::unordered_map<LoggingIdentifier, DltContext,
                     LoggingIdentifier::HashFunction,
                     std::equal_to<>,
                     score::cpp::pmr::polymorphic_allocator<
                         std::pair<const LoggingIdentifier, DltContext>>>
      contextMap_;

  /// \brief Map of all current open records.
  std::unordered_map<SlotIndex, DltContextData, std::hash<SlotIndex>,
                     std::equal_to<>,
                     score::cpp::pmr::polymorphic_allocator<
                         std::pair<const SlotIndex, DltContextData>>>
      contextDataMap_;

  std::unique_ptr<dlt::IDlt> dlt_;
};

} // namespace score::mw::log::detail

#endif // SCORE_MW_LOG_DLT_RECORDER_H_
