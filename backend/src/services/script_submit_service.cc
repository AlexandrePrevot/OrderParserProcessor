#include "services/script_submit_service.h"
#include <grpcpp/support/status.h>
#include <stdexcept>

grpc::Status
ScriptSubmitService::ScriptSubmit(grpc::ServerContext *context,
                                  const internal::ScriptSubmitRequest *request,
                                  internal::SynchronousReply *response) {
  try {
    if (!processor_) {
      throw std::runtime_error("processor has not been instantiated");
    }

    processor_(*request);
  } catch (const std::exception &except) {
    return grpc::Status(
        grpc::StatusCode::INTERNAL,
        std::string("CORE : Exception in the ScriptSubmit processing : ") + except.what());
  }

  return grpc::Status::OK;
}