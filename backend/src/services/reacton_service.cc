#include "services/reacton_service.h"

#include <iostream>
#include <google/protobuf/empty.pb.h>

ReactOnService::ReactOnService() : stop_(false) {
  channel_ = grpc::CreateChannel("localhost:50052",
                                 grpc::InsecureChannelCredentials());
  stub_ = internal::MarketDataService::NewStub(channel_);
  reader_thread_ = std::thread(&ReactOnService::ReadMarketDataStream, this);
}

ReactOnService::~ReactOnService() {
  stop_ = true;

  if (reader_thread_.joinable()) {
    reader_thread_.join();
  }
}

void ReactOnService::RegisterReaction(const std::string &instrument_id,
                                       int max_count,
                                       std::function<void()> callback) {
  reactions_.push_back(
      std::make_shared<Reaction>(instrument_id, max_count, callback));
}

void ReactOnService::WaitForCompletion() {
  if (reader_thread_.joinable()) {
    reader_thread_.join();
  }
}

bool ReactOnService::ShouldStopReading() {
  if (stop_) {
    return true;
  }

  for (const auto &reaction : reactions_) {
    if (reaction->max_count == -1 ||
        reaction->current_count < reaction->max_count) {
      return false;
    }
  }

  return true;
}

void ReactOnService::ReadMarketDataStream() {
  grpc::ClientContext context;
  google::protobuf::Empty request;

  std::unique_ptr<grpc::ClientReader<internal::PriceUpdate>> reader(
      stub_->StreamPrices(&context, request));

  internal::PriceUpdate update;

  while (reader->Read(&update)) {
    for (const auto &reaction : reactions_) {
      if (reaction->max_count != -1 &&
          reaction->current_count >= reaction->max_count) {
        continue;
      }

      reaction->callback();
      reaction->current_count++;
    }

    // Here we use TryCancel
    // because Finish() waits for
    // the server to ends its streaming
    // while we want to cancel the subscription
    // immediatly
    if (ShouldStopReading()) {
      context.TryCancel();
      break;
    }
  }

  grpc::Status status = reader->Finish();

  if (!status.ok() && status.error_code() != grpc::StatusCode::CANCELLED) {
    std::cerr << "StreamPrices RPC failed: " << status.error_message()
              << std::endl;
  }
}
