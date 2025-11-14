#ifndef SCORE_MW_LOG_DETAIL_DATA_ROUTER_DLT_WRAPPER_MOCK_H
#define SCORE_MW_LOG_DETAIL_DATA_ROUTER_DLT_WRAPPER_MOCK_H

#include "idlt_wrapper.hpp"
#include "gmock/gmock.h"

namespace score::mw::log::detail::dlt {

class DltMock : public IDlt {
public:
  DltMock() noexcept;
  ~DltMock() noexcept;

  MOCK_METHOD(DltReturnValue, RegisterApp, 
              (std::string_view app_id, std::string_view description), 
              (noexcept, const, override));
  
  MOCK_METHOD(DltReturnValue, UnregisterApp, (), (noexcept, const, override));
  
  MOCK_METHOD(DltReturnValue, RegisterContext,
              (DltContext &context, std::string_view context_id, std::string_view description), 
              (noexcept, override));
              
  MOCK_METHOD(DltReturnValue, UnregisterContext,
              (DltContext &context), 
              (noexcept, override));
              
  MOCK_METHOD(DltReturnValue, StartLog,
              (DltContext* context, DltContextData* record, DltLogLevelType log_level),
              (noexcept, override));
              
  MOCK_METHOD(DltReturnValue, FinishLog, 
              (DltContextData *record),
              (noexcept, override));

  MOCK_METHOD(DltReturnValue, LogBool, (DltContextData *record, bool data), (noexcept, override));
  MOCK_METHOD(DltReturnValue, LogUint8, (DltContextData *record, std::uint8_t data), (noexcept, override));
  MOCK_METHOD(DltReturnValue, LogInt8, (DltContextData *record, std::int8_t data), (noexcept, override));
  MOCK_METHOD(DltReturnValue, LogUint16, (DltContextData *record, std::uint16_t data), (noexcept, override));
  MOCK_METHOD(DltReturnValue, LogInt16, (DltContextData *record, std::int16_t data), (noexcept, override));
  MOCK_METHOD(DltReturnValue, LogUint32, (DltContextData *record, std::uint32_t data), (noexcept, override));
  MOCK_METHOD(DltReturnValue, LogInt32, (DltContextData *record, std::int32_t data), (noexcept, override));
  MOCK_METHOD(DltReturnValue, LogUint64, (DltContextData *record, std::uint64_t data), (noexcept, override));
  MOCK_METHOD(DltReturnValue, LogInt64, (DltContextData *record, std::int64_t data), (noexcept, override));
  MOCK_METHOD(DltReturnValue, LogFloat32, (DltContextData *record, float data), (noexcept, override));
  MOCK_METHOD(DltReturnValue, LogFloat64, (DltContextData *record, double data), (noexcept, override));
  MOCK_METHOD(DltReturnValue, LogString, (DltContextData *record, const char* data), (noexcept, override));
  MOCK_METHOD(DltReturnValue, LogUint8Formatted, (DltContextData *record, std::uint8_t data, DltFormatType format), (noexcept, override));
  MOCK_METHOD(DltReturnValue, LogUint16Formatted, (DltContextData *record, std::uint16_t data, DltFormatType format), (noexcept, override));
  MOCK_METHOD(DltReturnValue, LogUint32Formatted, (DltContextData *record, std::uint32_t data, DltFormatType format), (noexcept, override));
  MOCK_METHOD(DltReturnValue, LogUint64Formatted, (DltContextData *record, std::uint64_t data, DltFormatType format), (noexcept, override));
              
  MOCK_METHOD(DltReturnValue, LogArithmatic,
              (DltContextData &record, std::uint16_t data), 
              (noexcept, override));
};

} // namespace score::mw::log::detail::dlt

#endif // SCORE_MW_LOG_DETAIL_DATA_ROUTER_DLT_WRAPPER_MOCK_H
