#include "gtest/gtest.h"

#include "data_router_recorder.h"
#include "dlt/dlt_wrapper_mock.hpp"
#include "score/mw/log/configuration/configuration.h"
#include <score/memory.hpp>

namespace score::mw::log {

class DataRouterRecorderFixture : public ::testing::Test {
public:
  void SetUp() {
    recorder_ =
        std::make_unique<detail::DataRouterRecorder>(config_, mem_, dlt_);
  }

protected:
  detail::Configuration config_{};
  score::cpp::pmr::memory_resource *mem_ =
      score::cpp::pmr::get_default_resource();
  std::unique_ptr<detail::dlt::DltMock> dlt_{};
  std::unique_ptr<detail::DataRouterRecorder> recorder_;
};

TEST_F(DataRouterRecorderFixture, Constructor) {}
} // namespace score::mw::log
