#pragma once

#include <vector>
#include "messages/price_update.pb.h"

// Mock implementation that mimics grpc::ServerWriter<T> interface
// Cannot inherit from grpc::ServerWriter because it's marked final
// Use this for testing code that accepts grpc::ServerWriter<T>*
template <typename T>
class MockServerWriter {
public:
  MockServerWriter() : should_fail_(false), write_count_(0) {}

  bool Write(const T& msg, grpc::WriteOptions options) {
    return Write(msg);
  }

  bool Write(const T& msg) {
    if (should_fail_) {
      return false;
    }

    messages_.push_back(msg);
    ++write_count_;
    return true;
  }

  void SetShouldFail(bool fail) {
    should_fail_ = fail;
  }

  size_t GetMessageCount() const {
    return messages_.size();
  }

  const std::vector<T>& GetMessages() const {
    return messages_;
  }

  const T& GetLastMessage() const {
    return messages_.back();
  }

  void Clear() {
    messages_.clear();
    write_count_ = 0;
  }

private:
  bool should_fail_;
  size_t write_count_;
  std::vector<T> messages_;
};

using MockGrpcWriter = MockServerWriter<internal::PriceUpdate>;
