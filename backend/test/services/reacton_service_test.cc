#include "services/reacton_service.h"

#include <gtest/gtest.h>
#include <chrono>

TEST(ReactOnServiceTest, ConstructorDestructor) {
  ReactOnService service;
}

TEST(ReactOnServiceTest, RegisterReaction) {
  ReactOnService service;
  int callback_count = 0;

  service.RegisterReaction("TEST", 5, [&callback_count]() {
    callback_count++;
  });

  EXPECT_EQ(callback_count, 0);
}

TEST(ReactOnServiceTest, MultipleReactions) {
  ReactOnService service;
  int count1 = 0;
  int count2 = 0;

  service.RegisterReaction("AAPL", 3, [&count1]() { count1++; });
  service.RegisterReaction("GOOGL", 5, [&count2]() { count2++; });

  EXPECT_EQ(count1, 0);
  EXPECT_EQ(count2, 0);
}

TEST(ReactOnServiceTest, InfiniteReaction) {
  ReactOnService service;
  int callback_count = 0;

  service.RegisterReaction("TEST", -1, [&callback_count]() {
    callback_count++;
  });

  EXPECT_EQ(callback_count, 0);
}

TEST(ReactionTest, ConstructorInitialization) {
  int test_count = 0;
  Reaction reaction("AAPL", 10, [&test_count]() { test_count++; });

  EXPECT_EQ(reaction.instrument_id, "AAPL");
  EXPECT_EQ(reaction.max_count, 10);
  EXPECT_EQ(reaction.current_count.load(), 0);

  reaction.callback();
  EXPECT_EQ(test_count, 1);
}

TEST(ReactionTest, InfiniteCount) {
  Reaction reaction("TEST", -1, []() {});

  EXPECT_EQ(reaction.max_count, -1);
  EXPECT_EQ(reaction.current_count.load(), 0);
}

TEST(ReactOnServiceTest, WaitForCompletionNoReactions) {
  ReactOnService service;
  auto start = std::chrono::steady_clock::now();
  service.WaitForCompletion();
  auto end = std::chrono::steady_clock::now();

  auto duration = std::chrono::duration_cast<std::chrono::seconds>(end - start);
  EXPECT_LT(duration.count(), 10);
}

TEST(ReactOnServiceTest, ThreadSafeRegistration) {
  ReactOnService service;
  int count1 = 0;
  int count2 = 0;
  int count3 = 0;

  service.RegisterReaction("A", 5, [&count1]() { count1++; });
  service.RegisterReaction("B", 3, [&count2]() { count2++; });
  service.RegisterReaction("C", 10, [&count3]() { count3++; });

  EXPECT_EQ(count1, 0);
  EXPECT_EQ(count2, 0);
  EXPECT_EQ(count3, 0);
}
