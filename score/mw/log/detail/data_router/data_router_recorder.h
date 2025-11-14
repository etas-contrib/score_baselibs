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
  template <typename T, typename LogFunc>
  std::enable_if_t<
      std::is_same_v<std::invoke_result_t<LogFunc, DltContextData *, const T>,
                     DltReturnValue> ||
          std::is_same_v<
              std::invoke_result_t<LogFunc, DltContextData *, const T &>,
              DltReturnValue>,
      DltReturnValue>
  LogImpl(const SlotHandle &slot_handle, const T &data,
          LogFunc log_func) noexcept {

    auto it = contextDataMap_.find(
        static_cast<int>(slot_handle.GetSlotOfSelectedRecorder()));
    if (it != contextDataMap_.end()) {
      return log_func(&it->second, data);
    }

    return DLT_RETURN_ERROR;
  }

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
