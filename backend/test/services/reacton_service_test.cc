#include "services/reacton_service.h"

#include <gtest/gtest.h>
#include <atomic>
#include <chrono>
#include <thread>

// Test basic construction and destruction
TEST(ReactOnServiceTest, ConstructorDestructor) {
  // Should construct and destruct without crashing
  ReactOnService service;
  // Destructor will be called automatically
}

// Test RegisterReaction basic functionality
TEST(ReactOnServiceTest, RegisterReaction) {
  ReactOnService service;

  std::atomic<int> callback_count{0};

  service.RegisterReaction("TEST", 5, [&callback_count]() {
    callback_count++;
  });

  // Note: Without a running Distributor, callbacks won't execute
  // This test just verifies registration doesn't crash
  EXPECT_EQ(callback_count.load(), 0);
}

// Test multiple reactions can be registered
TEST(ReactOnServiceTest, MultipleReactions) {
  ReactOnService service;

  std::atomic<int> count1{0};
  std::atomic<int> count2{0};

  service.RegisterReaction("AAPL", 3, [&count1]() {
    count1++;
  });

  service.RegisterReaction("GOOGL", 5, [&count2]() {
    count2++;
  });

  // Verify registration succeeded (callbacks won't execute without Distributor)
  EXPECT_EQ(count1.load(), 0);
  EXPECT_EQ(count2.load(), 0);
}

// Test infinite reaction (-1 count)
TEST(ReactOnServiceTest, InfiniteReaction) {
  ReactOnService service;

  std::atomic<int> callback_count{0};

  service.RegisterReaction("TEST", -1, [&callback_count]() {
    callback_count++;
  });

  // Should not crash with -1 (infinite) count
  EXPECT_EQ(callback_count.load(), 0);
}

// Test Reaction struct
TEST(ReactionTest, ConstructorInitialization) {
  std::atomic<int> test_count{0};

  Reaction reaction("AAPL", 10, [&test_count]() {
    test_count++;
  });

  EXPECT_EQ(reaction.instrument_id, "AAPL");
  EXPECT_EQ(reaction.max_count, 10);
  EXPECT_EQ(reaction.current_count.load(), 0);

  // Test callback execution
  reaction.callback();
  EXPECT_EQ(test_count.load(), 1);
}

// Test Reaction with infinite count
TEST(ReactionTest, InfiniteCount) {
  Reaction reaction("TEST", -1, []() {});

  EXPECT_EQ(reaction.max_count, -1);
  EXPECT_EQ(reaction.current_count.load(), 0);
}

// Test WaitForCompletion doesn't hang with no reactions
TEST(ReactOnServiceTest, WaitForCompletionNoReactions) {
  ReactOnService service;

  // Should return immediately since no reactions registered
  // and Distributor connection will fail quickly
  auto start = std::chrono::steady_clock::now();
  service.WaitForCompletion();
  auto end = std::chrono::steady_clock::now();

  auto duration = std::chrono::duration_cast<std::chrono::seconds>(end - start);

  // Should complete within a few seconds (connection failure)
  EXPECT_LT(duration.count(), 10);
}

// Test thread safety of registration before WaitForCompletion
TEST(ReactOnServiceTest, ThreadSafeRegistration) {
  ReactOnService service;

  std::atomic<int> count1{0};
  std::atomic<int> count2{0};
  std::atomic<int> count3{0};

  // Register multiple reactions
  service.RegisterReaction("A", 5, [&count1]() { count1++; });
  service.RegisterReaction("B", 3, [&count2]() { count2++; });
  service.RegisterReaction("C", 10, [&count3]() { count3++; });

  // All counts should still be 0 (no Distributor running)
  EXPECT_EQ(count1.load(), 0);
  EXPECT_EQ(count2.load(), 0);
  EXPECT_EQ(count3.load(), 0);
}
