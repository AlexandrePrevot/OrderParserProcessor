#pragma once

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

  grpc::Status StreamPrices(
      grpc::ServerContext* context,
      const google::protobuf::Empty* request,
      grpc::ServerWriter<internal::PriceUpdate>* writer) override;

private:
  std::shared_ptr<PythonApiGtw> gateway_;

  long long call_count_ = 0;
  long long failed_call_count_ = 0;
};
