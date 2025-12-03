#include <chrono>
#include <gtest/gtest.h>
#include <vector>

#include "processors/common/timers.h"

using namespace std::chrono;

namespace {

constexpr seconds kShortDelay{1};
constexpr seconds kLongDelay{2};
constexpr microseconds kTolerance{500};

// heavy load tests - This is something we can improve with
// the thread pool (see timers.cc ::Run() comments)
constexpr microseconds kLargeTolerance{5000};

constexpr int kTimerCount = 4;
constexpr int kTimer0Repeat = 2;
constexpr int kTimer1Repeat = 1;
constexpr int kTimer2Repeat = 2;
constexpr int kTimer3Repeat = -1;

// Timer indices for clarity
enum TimerIndex { T0 = 0, T1 = 1, T2 = 2, T3 = 3 };

bool almost_equal(steady_clock::time_point t1, steady_clock::time_point t2,
                  steady_clock::duration tolerance) {
  return (t1 > t2 ? t1 - t2 : t2 - t1) <= tolerance;
}

std::string make_msg(const steady_clock::time_point &expected,
                     const steady_clock::time_point &actual,
                     const steady_clock::time_point &start,
                     const steady_clock::duration &tolerance) {
  auto diff = (expected > actual) ? expected - actual : actual - expected;
  std::ostringstream os;

  os << "Expected: " << duration_cast<nanoseconds>(expected - start).count()
     << " ns from now\n"
     << "Actual:   " << duration_cast<nanoseconds>(actual - start).count()
     << " ns from now\n"
     << "Diff:     " << duration_cast<nanoseconds>(diff).count() << " ns\n"
     << "Tolerance: " << duration_cast<nanoseconds>(tolerance).count() << " ns";

  return os.str();
}

steady_clock::time_point
expected_fire_time(const steady_clock::time_point &start, seconds delay,
                   int occurrence_index) {
  return start + delay * (occurrence_index + 1);
}

} // namespace

// -----------------------------------------------------------------------------
// Full multi-timer test
// -----------------------------------------------------------------------------
TEST(TimerTest, MultipleTimers) {
  TimerManager manager;
  std::vector<std::vector<steady_clock::time_point>> result(kTimerCount);
  const auto now = steady_clock::now();

  manager.CreateTimer([&] { result[T0].push_back(steady_clock::now()); },
                      kShortDelay, kTimer0Repeat);
  manager.CreateTimer([&] { result[T1].push_back(steady_clock::now()); },
                      kLongDelay, kTimer1Repeat);
  manager.CreateTimer([&] { result[T2].push_back(steady_clock::now()); },
                      kShortDelay, kTimer2Repeat);
  manager.CreateTimer([&] { result[T3].push_back(steady_clock::now()); },
                      kLongDelay,
                      kTimer3Repeat); // infinite / -1 meaning special

  EXPECT_EQ(manager.GetTimerCount(), manager.GetActiveTimerCount());
  EXPECT_EQ(manager.GetActiveTimerCount(), kTimerCount);

  for (const auto &v : result)
    EXPECT_TRUE(v.empty());

  manager.WaitTillLast(1);

  EXPECT_EQ(result[T0].size(), kTimer0Repeat);
  EXPECT_EQ(result[T1].size(), kTimer1Repeat);
  EXPECT_EQ(result[T2].size(), kTimer2Repeat);
  EXPECT_EQ(result[T3].size(), 1);

  // Validate fire timings
  auto validate = [&](int timerIndex, seconds delay, int repeats) {
    for (int i = 0; i < repeats; ++i) {
      auto expected = expected_fire_time(now, delay, i);
      auto actual = result[timerIndex][i];
      EXPECT_TRUE(almost_equal(actual, expected, kTolerance))
          << "Timer " << timerIndex << " index " << i << " fired incorrectly.\n"
          << make_msg(expected, actual, now, kTolerance);
    }
  };

  validate(T0, kShortDelay, kTimer0Repeat);
  validate(T1, kLongDelay, kTimer1Repeat);
  validate(T2, kShortDelay, kTimer2Repeat);
  validate(T3, kLongDelay, 1);

  EXPECT_EQ(manager.GetActiveTimerCount(), 1);
}

// -----------------------------------------------------------------------------
// Single timer test
// -----------------------------------------------------------------------------
TEST(TimerTest, SingleTimerFiresOnce) {
  TimerManager manager;
  steady_clock::time_point fired_time;

  const auto now = steady_clock::now();
  manager.CreateTimer([&] { fired_time = steady_clock::now(); }, kShortDelay,
                      1);

  manager.WaitTillLast(0);

  auto expected = now + kShortDelay;
  EXPECT_TRUE(almost_equal(expected, fired_time, kTolerance))
      << make_msg(expected, fired_time, now, kTolerance);
}

// -----------------------------------------------------------------------------
// Test where the system waits before starting
// -----------------------------------------------------------------------------
TEST(TimerTest, WaitBeforeCreatingTimer) {
  TimerManager manager;

  std::this_thread::sleep_for(kShortDelay);

  steady_clock::time_point fired_time;
  const auto now = steady_clock::now();

  manager.CreateTimer([&] { fired_time = steady_clock::now(); }, kShortDelay,
                      1);

  manager.WaitTillLast(0);

  auto expected = now + kShortDelay;
  EXPECT_TRUE(almost_equal(expected, fired_time, kTolerance))
      << make_msg(expected, fired_time, now, kTolerance);
}

// -----------------------------------------------------------------------------
// Stress test: 1000 timers with mixed millisecond delays
// Ensures full test completes well under 10 seconds.
// -----------------------------------------------------------------------------
TEST(TimerTest, HeavyLoadManyTimers) {
  constexpr int kNumTimers = 1000;

  // Delay groups
  constexpr milliseconds kFastMinDelay{1}; // 1–5 ms
  constexpr milliseconds kFastMaxOffset{5};
  constexpr milliseconds kMediumDelay{150};    // ~150 ms
  constexpr milliseconds kLongDelayBase{1000}; // 1s, 2s, 3s → 1000–3000 ms

  constexpr int kMaxRepeats = 3;

  TimerManager manager;
  std::vector<std::vector<steady_clock::time_point>> results(kNumTimers);

  const auto now = steady_clock::now();

  // Create a variety of timer configurations
  for (int i = 0; i < kNumTimers; ++i) {
    milliseconds delay_ms;

    if (i < 500) {
      // Fast timers: 1–5 ms
      delay_ms = kFastMinDelay + milliseconds(i % kFastMaxOffset.count());
    } else if (i < 800) {
      // Medium timers: ~150 ms
      delay_ms = kMediumDelay;
    } else {
      // Long timers: 1s–3s in ms (1000–3000ms)
      delay_ms = kLongDelayBase * (1 + (i % 3));
    }

    int repeats = 1 + (i % kMaxRepeats);

    manager.CreateTimer(
        [&, idx = i]() { results[idx].push_back(steady_clock::now()); },
        delay_ms, repeats);
  }

  // Wait for all timers (blocking)
  auto wait_start = steady_clock::now();
  manager.WaitTillLast(0);
  auto wait_end = steady_clock::now();

  EXPECT_LT(wait_end - wait_start, seconds(10))
      << "Timers took too long to complete (expected < 10 seconds).";

  // Validate number of firings
  for (int i = 0; i < kNumTimers; ++i) {
    int expected_repeats = 1 + (i % kMaxRepeats);
    EXPECT_EQ(results[i].size(), expected_repeats)
        << "Timer " << i << " fired wrong number of times.";
  }

  // Validate timing (only check first firing)
  for (int i = 0; i < kNumTimers; ++i) {
    if (results[i].empty())
      continue;

    milliseconds delay_ms;

    if (i < 500) {
      delay_ms = kFastMinDelay + milliseconds(i % kFastMaxOffset.count());
    } else if (i < 800) {
      delay_ms = kMediumDelay;
    } else {
      delay_ms = kLongDelayBase * (1 + (i % 3));
    }

    auto expected = now + delay_ms;
    auto actual = results[i][0];

    EXPECT_TRUE(almost_equal(actual, expected, kLargeTolerance))
        << "Timer " << i << " first firing outside expected window.\n"
        << make_msg(expected, actual, now, kLargeTolerance);
  }
}
