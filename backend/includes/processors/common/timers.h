#pragma once

#include <chrono>
#include <condition_variable>
#include <functional>
#include <memory>
#include <mutex>
#include <queue>
#include <thread>
#include <vector>

struct Task {
  std::chrono::steady_clock::time_point
      time_to_run;                    // time for which the task need to be ran
  std::chrono::milliseconds interval; // when repeating
  std::function<void()> task;         // the lambda function needed
  int repeat;                         // -1 means indefinitly
};

struct TimerCmp {
  bool operator()(const std::shared_ptr<Task> &t1,
                  const std::shared_ptr<Task> &t2) {
    return t1->time_to_run > t2->time_to_run;
  }
};

class TimerManager {
public:
  TimerManager();
  ~TimerManager();

  TimerManager(const TimerManager &) = delete;
  TimerManager &operator=(const TimerManager &) = delete;
  TimerManager(TimerManager &&) = delete;
  TimerManager &operator=(TimerManager &&) = delete;

  void CreateTimer(std::function<void()> task,
                   const std::chrono::milliseconds &delay, int repeat_count);

  // to use only if the thread is waiting for
  // a number of tasks to be finished (to wait for all timers rest = 0)
  // if you add others tasks during this waiting (even because of repeatition)
  // it will also wait for them to be finished
  // so it will wait until GetActiveTimerCount()
  // returns rest or less
  void WaitTillLast(long long rest);

  unsigned long long GetActiveTimerCount() {
    std::unique_lock<std::mutex> lock(timer_mtx_);
    return active_timer_count_;
  }

  unsigned long long GetTimerCount() {
    std::unique_lock<std::mutex> lock(timer_mtx_);
    return timer_count_;
  }

private:
  void Run();

  using TaskPtr = std::shared_ptr<Task>;
  std::priority_queue<TaskPtr, std::vector<TaskPtr>, TimerCmp> tasks_to_do_;

  // lock mutex before calling Push or Pop
  void inline PushTask(const std::chrono::steady_clock::time_point &time_to_run,
                       const std::chrono::milliseconds &interval,
                       const std::function<void()> &task, int repeat) {
    PushTaskPtr(
        std::make_shared<Task>(Task{time_to_run, interval, task, repeat}));
  }

  void inline PushTaskPtr(const TaskPtr& task) {
    tasks_to_do_.push(task);
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
  std::condition_variable wait_cond_var_;

  unsigned long long active_timer_count_;
  unsigned long long timer_count_;
  unsigned long long timer_limit_;
};
