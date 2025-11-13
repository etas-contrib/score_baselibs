#ifndef SCORE_MW_LOG_DLT_RECORDER_H_
#define SCORE_MW_LOG_DLT_RECORDER_H_

#include "score/mw/log/configuration/configuration.h"
#include "score/mw/log/detail/backend.h"
#include "score/mw/log/recorder.h"
#include "score/mw/log/slot_handle.h"
#include <dlt/dlt_types.h>
#include <dlt/dlt_user.h>
#include <score/memory.hpp>
#include <score/optional.hpp>
#include <unordered_map>
#include "dlt/idlt_wrapper.hpp"
#include <memory>

#include <iostream>

namespace score::mw::log::detail {

class DataRouterRecorder : public Recorder {
public:
  // Constructor that accepts a DLT implementation - for production use
  DataRouterRecorder(
      const Configuration &config,
      score::cpp::pmr::memory_resource *memory_resource) noexcept;

  // Constructor that accepts a custom DLT implementation - for testing/mocking
  DataRouterRecorder(
      const Configuration &config,
      score::cpp::pmr::memory_resource *memory_resource,
      std::unique_ptr<dlt::IDlt> dlt_implementation) noexcept;
  ~DataRouterRecorder() noexcept;

  score::cpp::optional<SlotHandle>
  StartRecord(const std::string_view context_id,
              const LogLevel log_level) noexcept;

  void StopRecord(const SlotHandle &slot) noexcept;

  void Log(const SlotHandle &, const bool data) noexcept;

  void Log(const SlotHandle &, const std::uint8_t) noexcept;
  void Log(const SlotHandle &, const std::int8_t) noexcept;

  void Log(const SlotHandle &, const std::uint16_t) noexcept;
  void Log(const SlotHandle &, const std::int16_t) noexcept;

  void Log(const SlotHandle &, const std::uint32_t) noexcept;
  void Log(const SlotHandle &, const std::int32_t) noexcept;

  void Log(const SlotHandle &, const std::uint64_t) noexcept;
  void Log(const SlotHandle &, const std::int64_t) noexcept;

  void Log(const SlotHandle &, const float) noexcept;
  void Log(const SlotHandle &, const double) noexcept;

  void Log(const SlotHandle &, const std::string_view) noexcept;

  void Log(const SlotHandle &, const LogHex8) noexcept;

  void Log(const SlotHandle &, const LogHex16) noexcept;

  void Log(const SlotHandle &, const LogHex32) noexcept;

  void Log(const SlotHandle &, const LogHex64) noexcept;

  void Log(const SlotHandle &, const LogBin8) noexcept;

  void Log(const SlotHandle &, const LogBin16) noexcept;

  void Log(const SlotHandle &, const LogBin32) noexcept;

  void Log(const SlotHandle &, const LogBin64) noexcept;

  void Log(const SlotHandle &, const LogRawBuffer) noexcept;

  void Log(const SlotHandle &, const LogSlog2Message) noexcept;

  bool IsLogEnabled(const LogLevel &,
                    const std::string_view context) const noexcept;

private:
template <typename T, typename LogFunc>
std::enable_if_t<
    std::is_same_v<
        std::invoke_result_t<LogFunc, DltContextData*, const T>,
        DltReturnValue
    > ||
    std::is_same_v<
        std::invoke_result_t<LogFunc, DltContextData*, const T&>,
        DltReturnValue
    >,
    DltReturnValue
>
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
                     std::equal_to<LoggingIdentifier>,
                     score::cpp::pmr::polymorphic_allocator<
                         std::pair<const LoggingIdentifier, DltContext>>>
      contextMap_;

  /// \brief Map of all current open records.
  std::unordered_map<SlotIndex, DltContextData, std::hash<SlotIndex>,
                     std::equal_to<SlotIndex>,
                     score::cpp::pmr::polymorphic_allocator<
                         std::pair<const SlotIndex, DltContextData>>>
      contextDataMap_;
  
  std::unique_ptr<dlt::IDlt> dlt_;
};

} // namespace score::mw::log::detail

#endif // SCORE_MW_LOG_DLT_RECORDER_H_
