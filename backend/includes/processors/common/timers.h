#pragma once

#include <chrono>
#include <condition_variable>
#include <functional>
#include <memory>
#include <mutex>
#include <queue>
#include <thread>
#include <vector>

// must consider failover, so a created timer should be persisted in DB

struct Task {
  std::chrono::steady_clock::time_point
      time_to_run;                    // time for which the task need to be ran
  std::chrono::milliseconds interval; // in case repeat = -1
  std::function<void()> task;         // the lambda function needed
  int repeat;                         // -1 means indefinitly
};

struct TimerCmp {
  bool operator()(const std::shared_ptr<Task> &t1,
                  const std::shared_ptr<Task> &t2) {
    return t1->time_to_run > t2->time_to_run;
  }
};

// must be a singleton
class TimerManager {
public:
  TimerManager();
  ~TimerManager();
  void CreateTimer(std::function<void()> task,
                   const std::chrono::milliseconds &delay, int repeat_count);

  unsigned long long GetActiveTimerCount() { return active_timer_count_; }

  unsigned long long GetTimerCount() { return timer_count_; }

private:
  void Run();

  using TaskPtr = std::shared_ptr<Task>;
  std::priority_queue<TaskPtr, std::vector<TaskPtr>, TimerCmp> tasks_to_do_;

  // lock mutex before calling
  void inline PushTask(const std::chrono::steady_clock::time_point &time_to_run,
                       const std::chrono::milliseconds &interval,
                       const std::function<void()> &task, int repeat) {
    tasks_to_do_.emplace(
        std::make_shared<Task>(Task{time_to_run, interval, task, repeat}));
    active_timer_count_++;
    timer_count_++;
  }

  void inline PopTask() {
    if (tasks_to_do_.empty())
      return;

    tasks_to_do_.pop();
    active_timer_count_--;
  }

  bool stop_;
  std::thread worker_;
  std::mutex timer_mtx_;
  std::condition_variable cond_var_;

  unsigned long long active_timer_count_;
  unsigned long long timer_count_;
  unsigned long long timer_limit_;
};
