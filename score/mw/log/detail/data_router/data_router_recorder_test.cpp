#include "gtest/gtest.h"

#include "data_router_recorder.h"
#include "dlt/dlt_wrapper_mock.hpp"
#include "score/mw/log/log_types.h"
#include "score/mw/log/configuration/configuration.h"
#include <dlt/dlt_types.h>
#include <score/memory.hpp>

using ::testing::_;
using ::testing::Return;

namespace score::mw::log {

class DataRouterRecorderFixture : public ::testing::Test {
public:
  void SetUp() {}

protected:
  detail::Configuration config_;
  score::cpp::pmr::memory_resource *mem_ =
      score::cpp::pmr::get_default_resource();
  detail::dlt::DltMock *dlt_; // Raw pointer for testing
  std::unique_ptr<detail::DataRouterRecorder> recorder_;
};

TEST_F(DataRouterRecorderFixture, RegisterFailure) {
  auto dlt_mock = std::make_unique<detail::dlt::DltMock>();
  dlt_ = dlt_mock.get();
  EXPECT_CALL(*dlt_, RegisterApp(_, _)).WillOnce(Return(DLT_RETURN_ERROR));
  recorder_ = std::make_unique<detail::DataRouterRecorder>(config_, mem_,
                                                           std::move(dlt_mock));
  EXPECT_CALL(*dlt_, UnregisterApp()).WillOnce(Return(DLT_RETURN_OK));
}

TEST_F(DataRouterRecorderFixture, RegisterSuccess) {
  auto dlt_mock = std::make_unique<detail::dlt::DltMock>();
  dlt_ = dlt_mock.get();
  EXPECT_CALL(*dlt_, RegisterApp(_, _)).WillOnce(Return(DLT_RETURN_OK));
  recorder_ = std::make_unique<detail::DataRouterRecorder>(config_, mem_,
                                                           std::move(dlt_mock));
  EXPECT_CALL(*dlt_, UnregisterApp()).WillOnce(Return(DLT_RETURN_OK));
}

class DataRouterRecorderConstFixture : public ::testing::Test {
public:
  void SetUp() {
    auto dlt_mock = std::make_unique<detail::dlt::DltMock>();
    dlt_ = dlt_mock.get(); // Keep raw pointer for test assertions
    EXPECT_CALL(*dlt_, RegisterApp(_, _)).WillOnce(Return(DLT_RETURN_OK));
    recorder_ = std::make_unique<detail::DataRouterRecorder>(
        config_, mem_, std::move(dlt_mock));
    EXPECT_CALL(*dlt_, UnregisterApp()).WillOnce(Return(DLT_RETURN_OK));
  }

  detail::Configuration config_;
  score::cpp::pmr::memory_resource *mem_ =
      score::cpp::pmr::get_default_resource();
  detail::dlt::DltMock *dlt_; // Raw pointer for testing
  std::unique_ptr<detail::DataRouterRecorder> recorder_;
};

TEST_F(DataRouterRecorderConstFixture, StartFailure) {}

class DataRouterRecorderParam
    : public testing::TestWithParam<
          std::pair<std::function<void(detail::DataRouterRecorder *)>,
                    std::function<void(detail::dlt::DltMock *)>>> {
public:
  void SetUp() {
    auto dlt_mock = std::make_unique<detail::dlt::DltMock>();
    dlt_ = dlt_mock.get(); // Keep raw pointer for test assertions
    EXPECT_CALL(*dlt_, RegisterApp(_, _)).WillOnce(Return(DLT_RETURN_OK));
    recorder_ = std::make_unique<detail::DataRouterRecorder>(
        config_, mem_, std::move(dlt_mock));
    EXPECT_CALL(*dlt_, UnregisterApp()).WillOnce(Return(DLT_RETURN_OK));
  }

  detail::Configuration config_;
  score::cpp::pmr::memory_resource *mem_ =
      score::cpp::pmr::get_default_resource();
  detail::dlt::DltMock *dlt_; // Raw pointer for testing
  std::unique_ptr<detail::DataRouterRecorder> recorder_;
};

TEST_P(DataRouterRecorderParam, SuccessLogging) {
  auto [call, mock] = GetParam();

  EXPECT_CALL(*dlt_, RegisterContext).WillOnce(Return(DLT_RETURN_OK));
  EXPECT_CALL(*dlt_, UnregisterContext).WillOnce(Return(DLT_RETURN_OK));
  EXPECT_CALL(*dlt_, StartLog).WillOnce(Return(DLT_RETURN_OK));
  EXPECT_CALL(*dlt_, FinishLog).WillOnce(Return(DLT_RETURN_OK));

  mock(dlt_);
  call(recorder_.get());
}

namespace {
constexpr auto LogType = [](detail::DataRouterRecorder *recorder, auto data) {
  auto slot = recorder->StartRecord("ctx", LogLevel::kFatal);
  ASSERT_TRUE(slot);

  recorder->Log(slot.value(), data);
  recorder->StopRecord(slot.value());
};
}

INSTANTIATE_TEST_SUITE_P(
    DataRouterLogAllTypes, DataRouterRecorderParam,
    ::testing::Values(
        std::make_pair([](detail::DataRouterRecorder *l) { LogType(l, true); },
                       [](detail::dlt::DltMock *m) {
                         EXPECT_CALL(*m, LogBool(_, _));
                       }),
        std::make_pair(
            [](detail::DataRouterRecorder *l) {
              LogType(l, static_cast<std::int8_t>(21));
            },
            [](detail::dlt::DltMock *m) { EXPECT_CALL(*m, LogInt8); }),
        std::make_pair(
            [](detail::DataRouterRecorder *l) {
              LogType(l, static_cast<std::uint8_t>(21));
            },
            [](detail::dlt::DltMock *m) { EXPECT_CALL(*m, LogUint8); }),
        std::make_pair(
            [](detail::DataRouterRecorder *l) {
              LogType(l, static_cast<std::int16_t>(21));
            },
            [](detail::dlt::DltMock *m) { EXPECT_CALL(*m, LogInt16); }),
        std::make_pair(
            [](detail::DataRouterRecorder *l) {
              LogType(l, static_cast<std::uint16_t>(21));
            },
            [](detail::dlt::DltMock *m) { EXPECT_CALL(*m, LogUint16); }),
        std::make_pair(
            [](detail::DataRouterRecorder *l) {
              LogType(l, static_cast<std::int32_t>(21));
            },
            [](detail::dlt::DltMock *m) { EXPECT_CALL(*m, LogInt32); }),
        std::make_pair(
            [](detail::DataRouterRecorder *l) {
              LogType(l, static_cast<std::uint32_t>(21));
            },
            [](detail::dlt::DltMock *m) { EXPECT_CALL(*m, LogUint32); }),
        std::make_pair(
            [](detail::DataRouterRecorder *l) {
              LogType(l, static_cast<std::int64_t>(21));
            },
            [](detail::dlt::DltMock *m) { EXPECT_CALL(*m, LogInt64); }),
        std::make_pair(
            [](detail::DataRouterRecorder *l) {
              LogType(l, static_cast<std::uint64_t>(21));
            },
            [](detail::dlt::DltMock *m) { EXPECT_CALL(*m, LogUint64(_, _)); }),
        std::make_pair(
            [](detail::DataRouterRecorder *l) {
              LogType(l, static_cast<float>(0.1));
            },
            [](detail::dlt::DltMock *m) { EXPECT_CALL(*m, LogFloat32); }),
        std::make_pair(
            [](detail::DataRouterRecorder *l) {
              LogType(l, static_cast<double>(0.1));
            },
            [](detail::dlt::DltMock *m) { EXPECT_CALL(*m, LogFloat64); }),
        std::make_pair(
            [](detail::DataRouterRecorder *l) {
              LogType(l, LogHex8{21});
            },
            [](detail::dlt::DltMock *m) { EXPECT_CALL(*m, LogUint8Formatted); }),
        std::make_pair(
            [](detail::DataRouterRecorder *l) {
              LogType(l, LogHex16{21});
            },
            [](detail::dlt::DltMock *m) { EXPECT_CALL(*m, LogUint16Formatted); }),
        std::make_pair(
            [](detail::DataRouterRecorder *l) {
              LogType(l, LogHex32{21});
            },
            [](detail::dlt::DltMock *m) { EXPECT_CALL(*m, LogUint32Formatted); }),
        std::make_pair(
            [](detail::DataRouterRecorder *l) {
              LogType(l, LogHex64{21});
            },
            [](detail::dlt::DltMock *m) { EXPECT_CALL(*m, LogUint64Formatted); }),
        std::make_pair(
            [](detail::DataRouterRecorder *l) {
              LogType(l, LogBin8{21});
            },
            [](detail::dlt::DltMock *m) { EXPECT_CALL(*m, LogUint8Formatted); }),

        std::make_pair(
            [](detail::DataRouterRecorder *l) {
              LogType(l, LogBin16{21});
            },
            [](detail::dlt::DltMock *m) { EXPECT_CALL(*m, LogUint16Formatted); }),
        std::make_pair(
            [](detail::DataRouterRecorder *l) {
              LogType(l, LogBin32{21});
            },
            [](detail::dlt::DltMock *m) { EXPECT_CALL(*m, LogUint16Formatted).Times(2); }),
        std::make_pair(
            [](detail::DataRouterRecorder *l) {
              LogType(l, LogBin64{21});
            },
            [](detail::dlt::DltMock *m) { EXPECT_CALL(*m, LogUint16Formatted).Times(4); }),
        std::make_pair(
            [](detail::DataRouterRecorder *l) {
              LogType(l, LogSlog2Message{21, "test"});
            },
            [](detail::dlt::DltMock *m) { EXPECT_CALL(*m, LogString); }),
        std::make_pair(
            [](detail::DataRouterRecorder *l) {
              LogType(l, LogSlog2Message{21, "test"});
            },
            [](detail::dlt::DltMock *m) { EXPECT_CALL(*m, LogString); }),

        std::make_pair(
            [](detail::DataRouterRecorder *l) {
              LogType(l, LogRawBuffer{"test"});
            },
            [](detail::dlt::DltMock *m) { EXPECT_CALL(*m, LogString); }),
        std::make_pair(
            [](detail::DataRouterRecorder *l) { LogType(l, std::string_view{"test"}); },
            [](detail::dlt::DltMock *m) {
              EXPECT_CALL(*m, LogString(_, _));
            })
));

} // namespace score::mw::log
