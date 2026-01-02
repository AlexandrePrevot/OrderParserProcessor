#pragma once

#include <atomic>
#include <functional>
#include <memory>
#include <string>
#include <thread>
#include <vector>

#include <grpcpp/grpcpp.h>
#include "messages/price_update.pb.h"
#include "services/marketdata.grpc.pb.h"

struct Reaction {
  std::string instrument_id;
  int max_count;
  std::atomic<int> current_count;
  std::function<void()> callback;

  Reaction(const std::string &id, int max, std::function<void()> cb)
      : instrument_id(id), max_count(max), current_count(0), callback(cb) {}
};

class ReactOnService {
public:
  ReactOnService();
  ~ReactOnService();

  ReactOnService(const ReactOnService &) = delete;
  ReactOnService &operator=(const ReactOnService &) = delete;
  ReactOnService(ReactOnService &&) = delete;
  ReactOnService &operator=(ReactOnService &&) = delete;

  void RegisterReaction(const std::string &instrument_id, int max_count,
                        std::function<void()> callback);

  void WaitForCompletion();

private:
  void ReadMarketDataStream();
  bool ShouldStopReading();

  std::shared_ptr<grpc::Channel> channel_;
  std::unique_ptr<internal::MarketDataService::Stub> stub_;
  std::thread reader_thread_;
  std::atomic<bool> stop_;

  std::vector<std::shared_ptr<Reaction>> reactions_;
};
