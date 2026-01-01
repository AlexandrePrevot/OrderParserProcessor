#include "processors/common/reacton_manager.h"

#include <iostream>
#include <google/protobuf/empty.pb.h>

ReactOnManager::ReactOnManager() : stop_(false) {
  // Create channel to Distributor server
  channel_ = grpc::CreateChannel("localhost:50052",
                                 grpc::InsecureChannelCredentials());

  // Create stub for MarketDataService
  stub_ = internal::MarketDataService::NewStub(channel_);

  // Start background thread to read market data stream
  reader_thread_ = std::thread(&ReactOnManager::ReadMarketDataStream, this);
}

ReactOnManager::~ReactOnManager() {
  stop_ = true;

  if (reader_thread_.joinable()) {
    reader_thread_.join();
  }
}

void ReactOnManager::RegisterReaction(const std::string &instrument_id,
                                       int max_count,
                                       std::function<void()> callback) {
  reactions_.push_back(
      std::make_shared<Reaction>(instrument_id, max_count, callback));
}

void ReactOnManager::WaitForCompletion() {
  if (reader_thread_.joinable()) {
    reader_thread_.join();
  }
}

bool ReactOnManager::ShouldStopReading() {
  // Stop if externally requested
  if (stop_) {
    return true;
  }

  // Stop if all reactions have completed their max_count
  for (const auto &reaction : reactions_) {
    // If any reaction is infinite (-1) or hasn't reached max, keep reading
    if (reaction->max_count == -1 ||
        reaction->current_count < reaction->max_count) {
      return false;
    }
  }

  // All reactions are done
  return true;
}

void ReactOnManager::ReadMarketDataStream() {
  grpc::ClientContext context;
  google::protobuf::Empty request;

  std::unique_ptr<grpc::ClientReader<internal::PriceUpdate>> reader(
      stub_->StreamPrices(&context, request));

  internal::PriceUpdate update;

  while (reader->Read(&update)) {
    // Check if we should stop reading
    if (ShouldStopReading()) {
      break;
    }

    // Execute all registered reaction callbacks
    for (const auto &reaction : reactions_) {
      // Check if this reaction should still execute
      if (reaction->max_count != -1 &&
          reaction->current_count >= reaction->max_count) {
        continue;  // Skip this reaction
      }

      // Execute the callback
      reaction->callback();

      // Increment counter
      reaction->current_count++;
    }
  }

  grpc::Status status = reader->Finish();

  if (!status.ok()) {
    std::cerr << "StreamPrices RPC failed: " << status.error_message()
              << std::endl;
  }
}
