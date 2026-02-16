#include <gtest/gtest.h>
#include "python_api_gtw.h"

class PythonApiGtwTest : public ::testing::Test {
protected:
  void SetUp() override {
    gateway_ = std::make_unique<PythonApiGtw>();
  }

  void TearDown() override {
    gateway_.reset();
  }

  std::unique_ptr<PythonApiGtw> gateway_;
};

TEST_F(PythonApiGtwTest, GatewayInitializes) {
  EXPECT_NE(gateway_, nullptr);
  EXPECT_FALSE(gateway_->IsRunning());
}

TEST_F(PythonApiGtwTest, SubscribeReturnsValidSubscription) {
  auto subscription = gateway_->Subscribe();

  EXPECT_NE(subscription, nullptr);
  EXPECT_TRUE(subscription->active.load());
}

TEST_F(PythonApiGtwTest, UnsubscribeDoesNotCrash) {
  auto subscription = gateway_->Subscribe();
  EXPECT_NO_THROW(gateway_->Unsubscribe(subscription));
}

TEST_F(PythonApiGtwTest, MultipleSubscriptions) {
  auto sub1 = gateway_->Subscribe();
  auto sub2 = gateway_->Subscribe();
  auto sub3 = gateway_->Subscribe();

  EXPECT_NE(sub1, sub2);
  EXPECT_NE(sub2, sub3);

  gateway_->Unsubscribe(sub2);
  gateway_->Unsubscribe(sub1);
  gateway_->Unsubscribe(sub3);
}

TEST_F(PythonApiGtwTest, DestructorCleansUp) {
  for (int i = 0; i < 5; ++i) {
    auto temp_gateway = std::make_unique<PythonApiGtw>();
  }

  SUCCEED();
}

// Note: Testing Start() and actual streaming requires either:
// 1. A real Python gateway running on port 9000
// 2. Refactoring to inject the socket/transport layer
