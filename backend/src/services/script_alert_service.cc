#include "services/script_alert_service.h"

#include <google/protobuf/empty.pb.h>
#include <google/protobuf/util/time_util.h>

#include "messages/script_alert_notif.pb.h"

ScriptAlertService::ScriptAlertService() {
  channel_ = grpc::CreateChannel("localhost:50053",
                                 grpc::InsecureChannelCredentials());
  stub_ = internal::ScriptToApi::NewStub(channel_);
}

ScriptAlertService::~ScriptAlertService() = default;

void ScriptAlertService::SendAlert(const std::string &user,
                                   const std::string &message) {
  grpc::ClientContext context;
  internal::ScriptAlertNotif request;
  google::protobuf::Empty response;

  request.set_user(user);
  request.set_message(message);
  request.set_script_title("FiScript");

  *request.mutable_creation_time() =
      google::protobuf::util::TimeUtil::GetCurrentTime();

  grpc::Status status = stub_->ScriptAlert(&context, request, &response);

  if (!status.ok()) {
    std::cout << "Failed to send alert: " << status.error_message()
              << std::endl;
  }
}