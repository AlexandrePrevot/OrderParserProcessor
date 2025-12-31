#pragma once

#include <grpcpp/server_context.h>

class MockServerContext : public grpc::ServerContext {
public:
  MockServerContext() = default;
  ~MockServerContext() override = default;
};
