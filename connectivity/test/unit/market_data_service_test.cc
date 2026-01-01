#include <gtest/gtest.h>
#include "services/market_data_service.h"

class MarketDataServiceTest : public ::testing::Test {
protected:
  void SetUp() override {
    service_ = std::make_unique<MarketDataService>();
  }

  void TearDown() override {
    service_.reset();
  }

  std::unique_ptr<MarketDataService> service_;
};

TEST_F(MarketDataServiceTest, ServiceInitializes) {
  EXPECT_NE(service_, nullptr);
}

// Note: Full testing of StreamPrices requires either:
// 1. A real Python gateway running on port 9000 (integration test)
// 2. Refactoring PythonApiGtw to accept injected socket dependency
// 3. Using test doubles for the socket connection
//
// Additionally, grpc::ServerWriter is marked 'final' so it cannot be mocked
// via inheritance. Testing the streaming behavior requires refactoring
// for dependency injection or integration testing.
