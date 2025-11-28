#include "processors/common/timers.h"
#include <chrono>
#include <iostream>

TimerManager::TimerManager() {
  stop_ = false;
  active_timer_count_ = 0;
  timer_count_ = 0;
  timer_limit_ = 100;
  worker_ = std::thread(&TimerManager::Run, this);
}

TimerManager::~TimerManager() {
  {
    std::unique_lock<std::mutex> lock(timer_mtx_);
    stop_ = true;
  }

  cond_var_.notify_one();
  worker_.join();
}

void TimerManager::CreateTimer(std::function<void()> task,
                               const std::chrono::milliseconds &delay,
                               int repeat_count) {
  std::unique_lock<std::mutex> lock(timer_mtx_);

  PushTask(std::chrono::steady_clock::now() + delay, delay, task, repeat_count);
  lock.unlock();
  cond_var_.notify_one();
}

void TimerManager::WaitTillLast(long long rest) {
  std::unique_lock<std::mutex> lock(timer_mtx_);

  while (active_timer_count_ > rest) {
    wait_cond_var_.wait(lock,
                        [this, rest]() { return active_timer_count_ <= rest; });
  }
}

void TimerManager::Run() {
  std::unique_lock<std::mutex> lock(timer_mtx_);

  while (!stop_) {
    if (tasks_to_do_.empty()) {
      cond_var_.wait(lock, [this]() { return stop_ || !tasks_to_do_.empty(); });
      continue;
    }

    const auto now = std::chrono::steady_clock::now();

    if (now < tasks_to_do_.top()->time_to_run) {
      cond_var_.wait_until(lock, tasks_to_do_.top()->time_to_run, [this]() {
        return stop_ || std::chrono::steady_clock::now() >=
                            tasks_to_do_.top()->time_to_run;
      });
      continue;
    }

    const auto job = tasks_to_do_.top();
    PopTask();

    // push the repeat before doing the job
    // because WaitTillFinished() can have spurious wake
    // during execution of a repeating job
    if (job->repeat == -1 || --job->repeat > 0) {
      PushTask(job->time_to_run + job->interval, job->interval, job->task,
               job->repeat);
    }

    // this is not correct
    // we should create a new thread for each task being done
    // because a task can take a long time to be done, so the other tasks
    // might take a few time before being done
    // should do it once the thread pool is done
    lock.unlock();
    wait_cond_var_.notify_all();
    job->task();
    lock.lock();
  }
}