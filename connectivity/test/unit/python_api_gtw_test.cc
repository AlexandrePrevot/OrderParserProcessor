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
}

TEST_F(PythonApiGtwTest, ConnectToGtwHandlesNullWriter) {
  bool result = gateway_->ConnectToGtw(nullptr);

  EXPECT_FALSE(result);
}

TEST_F(PythonApiGtwTest, ActivateDoesNotCrash) {
  EXPECT_NO_THROW(gateway_->Activate());
}

TEST_F(PythonApiGtwTest, DestructorCleansUpThreads) {
  // Create and destroy gateway multiple times
  // This tests that destructor properly joins threads
  for (int i = 0; i < 5; ++i) {
    auto temp_gateway = std::make_unique<PythonApiGtw>();
    // Destructor should clean up without hanging
  }

  SUCCEED();
}

// Note: Testing actual socket reading and JSON parsing requires either:
// 1. A mock socket implementation (requires refactoring PythonApiGtw)
// 2. A real test TCP server on port 9000
// 3. Dependency injection for the socket/reader
//
// Additionally, grpc::ServerWriter is marked 'final' so it cannot be mocked.
// To fully test parsing logic and streaming, refactor for dependency injection.
