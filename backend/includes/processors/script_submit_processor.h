#pragma once

#include "common/timers.h"
#include "messages/script_submit.pb.h"
#include "messages/synchronous_reply.pb.h"

class ScriptSubmitProcessor {
public:
  bool Process(const internal::ScriptSubmitRequest &script_submit_request);

  [[nodiscard]] std::shared_ptr<internal::SynchronousReply>
  GetSynchronousReply() {
    return synchronous_reply_;
  }

private:
  std::shared_ptr<internal::SynchronousReply> synchronous_reply_;

  bool Parse(const std::string &code);
  bool ScheduleCommand(const std::vector<std::string> &arguments);

  TimerManager timer_manager_;
};
