#pragma once

#include <functional>
#include <memory>
#include <grpcpp/grpcpp.h>
#include <grpcpp/support/status.h>

#include "google/protobuf/empty.pb.h"
#include "services/marketdata.grpc.pb.h"
#include "messages/price_update.pb.h"
#include "python_api_gtw.h"

class MarketDataService final : public internal::MarketDataService::Service {
public:
  MarketDataService();
  ~MarketDataService() = default;

  // Server-streaming RPC
  grpc::Status StreamPrices(
      grpc::ServerContext* context,
      const google::protobuf::Empty* request,
      grpc::ServerWriter<internal::PriceUpdate>* writer) override;

private:
  // Python gateway for connecting to market data source
  std::unique_ptr<PythonApiGtw> python_gateway_;

  long long call_count_ = 0;
  long long failed_call_count_ = 0;
  bool ready_to_serve_ = true;
};
