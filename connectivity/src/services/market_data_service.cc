#include "services/market_data_service.h"

#include <chrono>
#include <google/protobuf/timestamp.pb.h>
#include <iostream>
#include <stdexcept>
#include <thread>

MarketDataService::MarketDataService()
    : gateway_(std::make_shared<PythonApiGtw>()) {
  gateway_->Start();
  std::cout << "MarketDataService initialized" << std::endl;
}

grpc::Status MarketDataService::StreamPrices(
    grpc::ServerContext *context, const google::protobuf::Empty * /*request*/,
    grpc::ServerWriter<internal::PriceUpdate> *writer) {

  ++call_count_;

  std::cout << "Client connected to StreamPrices (call #" << call_count_ << ")"
            << std::endl;

  try {
    if (!gateway_) {
      throw std::runtime_error("Python gateway not initialized");
    }

    auto subscription = gateway_->Subscribe();

    while (!context->IsCancelled() && subscription->active.load()) {
      MarketDataPoint data_point;

      if (subscription->queue.pop(data_point)) {
        internal::PriceUpdate price_update;
        price_update.set_price(data_point.price);
        price_update.set_quantity(data_point.quantity);
        price_update.set_instrument_id(data_point.instrument_id);

        auto *timestamp = price_update.mutable_timestamp();
        timestamp->set_seconds(data_point.timestamp_seconds);
        timestamp->set_nanos(data_point.timestamp_nanos);

        if (!writer->Write(price_update)) {
          std::cerr << "Failed to write to gRPC stream (client disconnected)"
                    << std::endl;
          break;
        } else {
          std::cout << "Sent price update: " << data_point.instrument_id
                    << " price=" << data_point.price
                    << " quantity=" << data_point.quantity << std::endl;
        }
      } else {
        std::this_thread::sleep_for(std::chrono::microseconds(100));
      }
    }

    gateway_->Unsubscribe(subscription);

    std::cout << "StreamPrices completed for client" << std::endl;

  } catch (const std::exception &except) {
    ++failed_call_count_;

    std::cerr << "Exception in StreamPrices: " << except.what() << std::endl;

    return grpc::Status(
        grpc::StatusCode::INTERNAL,
        std::string("Exception in MarketData StreamPrices: ") + except.what());
  }

  return grpc::Status::OK;
}
