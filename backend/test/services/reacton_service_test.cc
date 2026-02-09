#include "services/reacton_service.h"

#include <gtest/gtest.h>
#include <chrono>

#include "messages/price_update.pb.h"

TEST(ReactOnServiceTest, ConstructorDestructor) {
  ReactOnService service;
}

TEST(ReactOnServiceTest, RegisterReaction) {
  ReactOnService service;
  int callback_count = 0;

  service.RegisterReaction("TEST", 5, [&callback_count](const internal::PriceUpdate &) {
    callback_count++;
  });

  EXPECT_EQ(callback_count, 0);
}

TEST(ReactOnServiceTest, MultipleReactions) {
  ReactOnService service;
  int count1 = 0;
  int count2 = 0;

  service.RegisterReaction("AAPL", 3, [&count1](const internal::PriceUpdate &) { count1++; });
  service.RegisterReaction("GOOGL", 5, [&count2](const internal::PriceUpdate &) { count2++; });

  EXPECT_EQ(count1, 0);
  EXPECT_EQ(count2, 0);
}

TEST(ReactOnServiceTest, InfiniteReaction) {
  ReactOnService service;
  int callback_count = 0;

  service.RegisterReaction("TEST", -1, [&callback_count](const internal::PriceUpdate &) {
    callback_count++;
  });

  EXPECT_EQ(callback_count, 0);
}

TEST(ReactionTest, ConstructorInitialization) {
  int test_count = 0;
  Reaction reaction("AAPL", 10, [&test_count](const internal::PriceUpdate &) { test_count++; });

  EXPECT_EQ(reaction.instrument_id, "AAPL");
  EXPECT_EQ(reaction.max_count, 10);
  EXPECT_EQ(reaction.current_count.load(), 0);

  internal::PriceUpdate dummy_update;
  reaction.callback(dummy_update);
  EXPECT_EQ(test_count, 1);
}

TEST(ReactionTest, InfiniteCount) {
  Reaction reaction("TEST", -1, [](const internal::PriceUpdate &) {});

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

  service.RegisterReaction("A", 5, [&count1](const internal::PriceUpdate &) { count1++; });
  service.RegisterReaction("B", 3, [&count2](const internal::PriceUpdate &) { count2++; });
  service.RegisterReaction("C", 10, [&count3](const internal::PriceUpdate &) { count3++; });

  EXPECT_EQ(count1, 0);
  EXPECT_EQ(count2, 0);
  EXPECT_EQ(count3, 0);
}
