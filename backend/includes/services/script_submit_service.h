#pragma once

#include <grpcpp/grpcpp.h>
#include <grpcpp/support/status.h>

#include "messages/script_submit.pb.h"
#include "services/api_to_core.grpc.pb.h"
#include "services/api_to_core.pb.h"

class ScriptSubmitService final : public internal::ApiToCore::Service {
public:
  grpc::Status ScriptSubmit(grpc::ServerContext * /*context*/,
                            const internal::ScriptSubmitRequest * /*request*/,
                            internal::SynchronousReply * /*response*/);

private:
  long long call_count_;
  long long failed_call_count_;
  bool ready_to_serve_;
};
