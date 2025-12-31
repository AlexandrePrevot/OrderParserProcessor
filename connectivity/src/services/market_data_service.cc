#include "services/market_data_service.h"
#include <iostream>
#include <stdexcept>

MarketDataService::MarketDataService()
    : python_gateway_(std::make_unique<PythonApiGtw>()) {
  std::cout << "MarketDataService initialized" << std::endl;
}

grpc::Status MarketDataService::StreamPrices(
    grpc::ServerContext* context,
    const google::protobuf::Empty* /*request*/,
    grpc::ServerWriter<internal::PriceUpdate>* writer) {

  ++call_count_;

  std::cout << "Client connected to StreamPrices (call #" << call_count_ << ")" << std::endl;

  try {
    if (!python_gateway_) {
      throw std::runtime_error("Python gateway not initialized");
    }

    // Connect to Python gateway and start streaming prices
    // This will block until the connection closes
    bool success = python_gateway_->ConnectToGtw(writer);

    if (!success) {
      ++failed_call_count_;
      return grpc::Status(
          grpc::StatusCode::INTERNAL,
          "Failed to connect to Python API Gateway");
    }

    std::cout << "StreamPrices completed successfully" << std::endl;

  } catch (const std::exception& except) {
    ++failed_call_count_;

    std::cerr << "Exception in StreamPrices: " << except.what() << std::endl;

    return grpc::Status(
        grpc::StatusCode::INTERNAL,
        std::string("Exception in MarketData StreamPrices: ") + except.what());
  }

  return grpc::Status::OK;
}
