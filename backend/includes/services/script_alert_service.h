#include <grpcpp/grpcpp.h>
#include "services/script_to_api.grpc.pb.h"

class ScriptAlertService {
public:
  ScriptAlertService();
  ~ScriptAlertService();

  ScriptAlertService(const ScriptAlertService &) = delete;
  ScriptAlertService &operator=(const ScriptAlertService &) = delete;
  ScriptAlertService(ScriptAlertService &&) = delete;
  ScriptAlertService &operator=(ScriptAlertService &&) = delete;

  void SendAlert(const std::string &message);


private:

  std::shared_ptr<grpc::Channel> channel_;
  std::unique_ptr<internal::ScriptToApi::Stub> stub_;

};
