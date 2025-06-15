#include <grpcpp/grpcpp.h>
#include <grpcpp/support/status.h>

#include "generated/cpp/api_to_core.grpc.pb.h"

class ScriptSubmitService {

  class ScriptSubmitServiceImpl {
    ::grpc::Status SubmitScript(::grpc::ServerContext * /*context*/,
                                const ::internal::ScriptSubmit * /*request*/,
                                ::internal::SynchronousReply * /*response*/);
  };

private:
  long long call_count_;
  long long failed_call_count_;
  bool ready_to_serve_;
};
