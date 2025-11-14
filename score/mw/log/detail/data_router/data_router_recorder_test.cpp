#include "gtest/gtest.h"

#include "data_router_recorder.h"
#include "dlt/dlt_wrapper_mock.hpp"
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
}

TEST_F(DataRouterRecorderFixture, RegisterSuccess) {
  auto dlt_mock = std::make_unique<detail::dlt::DltMock>();
  dlt_ = dlt_mock.get();
  EXPECT_CALL(*dlt_, RegisterApp(_, _)).WillOnce(Return(DLT_RETURN_OK));
  recorder_ = std::make_unique<detail::DataRouterRecorder>(config_, mem_,
                                                           std::move(dlt_mock));
}

class DataRouterRecorderConstFixture : public ::testing::Test {
public:
  void SetUp() {
    auto dlt_mock = std::make_unique<detail::dlt::DltMock>();
    dlt_ = dlt_mock.get(); // Keep raw pointer for test assertions
    EXPECT_CALL(*dlt_, RegisterApp(_, _)).WillOnce(Return(DLT_RETURN_OK));
    recorder_ = std::make_unique<detail::DataRouterRecorder>(
        config_, mem_, std::move(dlt_mock));
  }

protected:
  detail::Configuration config_;
  score::cpp::pmr::memory_resource *mem_ =
      score::cpp::pmr::get_default_resource();
  detail::dlt::DltMock *dlt_; // Raw pointer for testing
  std::unique_ptr<detail::DataRouterRecorder> recorder_;
};

TEST_F(DataRouterRecorderConstFixture, StartFailure) {

}

} // namespace score::mw::log
