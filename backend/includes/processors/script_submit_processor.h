#pragma once

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
};
