#include <iostream>

#include "handlers/script_submit_handler.h"
#include "processors/script_submit_processor.h"
#include "services/script_submit_service.h"

void RunServer() {
  const std::string server_address("0.0.0.0:50051");
  ScriptSubmitHandler script_submit_handler;

  grpc::ServerBuilder builder;
  builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
  builder.RegisterService(script_submit_handler.GetScriptSubmitServicePtr());

  std::unique_ptr<grpc::Server> server(builder.BuildAndStart());
  server->Wait();
}

int main() {
  std::cout << "hello world!" << std::endl;
  RunServer();
  return 0;
}
