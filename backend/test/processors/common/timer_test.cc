#include <chrono>
#include <gtest/gtest.h>
#include <vector>

#include "processors/common/timers.h"

bool almost_equal(std::chrono::steady_clock::time_point time1,
                  std::chrono::steady_clock::time_point time2,
                  std::chrono::steady_clock::duration tolerance) {
  return (time1 > time2 ? time1 - time2 : time2 - time1) <= tolerance;
}

TEST(TimerTest, TimerTest) {
  TimerManager m;
  std::vector<std::vector<std::chrono::time_point<std::chrono::steady_clock>>>
      result(4);
  const auto now = std::chrono::steady_clock::now();

  m.CreateTimer(
      [&result]() { result[0].push_back(std::chrono::steady_clock::now()); },
      std::chrono::seconds(1), 2);

  m.CreateTimer(
      [&result]() { result[1].push_back(std::chrono::steady_clock::now()); },
      std::chrono::seconds(2), 1);

  m.CreateTimer(
      [&result]() { result[2].push_back(std::chrono::steady_clock::now()); },
      std::chrono::seconds(1), 2);

  m.CreateTimer(
      [&result]() { result[3].push_back(std::chrono::steady_clock::now()); },
      std::chrono::seconds(2), -1);

  auto active_timer = m.GetActiveTimerCount();
  const auto timer = m.GetTimerCount();

  EXPECT_EQ(timer, active_timer);
  EXPECT_EQ(active_timer, 4);

  EXPECT_TRUE(result[0].empty());
  EXPECT_TRUE(result[1].empty());
  EXPECT_TRUE(result[2].empty());
  EXPECT_TRUE(result[3].empty());

  std::this_thread::sleep_for(std::chrono::seconds(5));

  EXPECT_EQ(2, result[0].size());
  EXPECT_EQ(1, result[1].size());
  EXPECT_EQ(2, result[2].size());
  EXPECT_EQ(2, result[3].size());

  using namespace std::chrono;

  auto tolerance = microseconds{500};

  auto make_msg = [&](auto expected, auto actual) {
    auto diff = (expected > actual) ? expected - actual : actual - expected;
    std::ostringstream os;
    os << "Expected: " << duration_cast<nanoseconds>(expected - now).count()
       << " ns from now\n"
       << "Actual:   " << duration_cast<nanoseconds>(actual - now).count()
       << " ns from now\n"
       << "Diff:     " << duration_cast<nanoseconds>(diff).count() << " ns\n"
       << "Tolerance: " << duration_cast<nanoseconds>(tolerance).count()
       << " ns";
    return os.str();
  };

  // result[0]
  for (int i = 0; i < 2; ++i) {
    auto expected = now + seconds(1) * (i + 1);
    auto actual = result[0][i];
    EXPECT_TRUE(almost_equal(actual, expected, tolerance))
        << "Timer 0 index " << i << " fired at wrong time:\n"
        << make_msg(expected, actual);
  }

  // result[1]
  {
    auto expected = now + seconds(2);
    auto actual = result[1][0];
    EXPECT_TRUE(almost_equal(actual, expected, tolerance))
        << "Timer 1 fired at wrong time:\n"
        << make_msg(expected, actual);
  }

  // result[2]
  for (int i = 0; i < 2; ++i) {
    auto expected = now + seconds(1) * (i + 1);
    auto actual = result[2][i];
    EXPECT_TRUE(almost_equal(actual, expected, tolerance))
        << "Timer 2 index " << i << " fired at wrong time:\n"
        << make_msg(expected, actual);
  }

  // result[3]
  for (int i = 0; i < 2; ++i) {
    auto expected = now + seconds(2) * (i + 1);
    auto actual = result[3][i];
    EXPECT_TRUE(almost_equal(actual, expected, tolerance))
        << "Timer 3 index " << i << " fired at wrong time:\n"
        << make_msg(expected, actual);
  }

  active_timer = m.GetActiveTimerCount();

  EXPECT_EQ(1, active_timer);
}