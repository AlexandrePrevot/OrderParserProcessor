#include <iostream>
#include <memory>
#include <grpcpp/grpcpp.h>

#include "services/market_data_service.h"

void RunServer() {
  const std::string server_address("0.0.0.0:50052");
  MarketDataService market_data_service;

  grpc::ServerBuilder builder;
  builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
  builder.RegisterService(&market_data_service);

  std::unique_ptr<grpc::Server> server(builder.BuildAndStart());
  std::cout << "Market data server listening on " << server_address << std::endl;
  server->Wait();
}

int main() {
  RunServer();
  return 0;
}