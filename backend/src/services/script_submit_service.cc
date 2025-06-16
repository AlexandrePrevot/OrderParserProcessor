#include "services/script_submit_service.h"
#include <grpcpp/support/status.h>

grpc::Status ScriptSubmitService::ScriptSubmit(grpc::ServerContext * context,
                            const internal::ScriptSubmitRequest * request,
                            internal::SynchronousReply * response) {

    return grpc::Status::OK;
}