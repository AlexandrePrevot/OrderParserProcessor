#pragma once

#include <atomic>
#include <thread>
#include <grpcpp/grpcpp.h>
#include <boost/lockfree/spsc_queue.hpp>
#include "messages/price_update.pb.h"
#include "market_data_point.h"

class PythonApiGtw {
public:
    PythonApiGtw();
    ~PythonApiGtw();

    // Prevent copying (threads are not copyable)
    PythonApiGtw(const PythonApiGtw&) = delete;
    PythonApiGtw& operator=(const PythonApiGtw&) = delete;

    // start to do whatever is necessary with the data receives
    void Activate();

    // Connect to gateway and stream price updates via gRPC
    bool ConnectToGtw(grpc::ServerWriter<internal::PriceUpdate>* writer);

private:
  bool ConnectThenSend();

  // Thread function to read from socket and parse JSON
  void SocketReaderThread();

  // Thread function to send PriceUpdates via gRPC
  void GrpcWriterThread(grpc::ServerWriter<internal::PriceUpdate>* writer);

  bool active_;

  // Lock-free queue for passing market data between threads
  // Uses POD struct (trivially copyable) - no heap allocation needed!
  boost::lockfree::spsc_queue<MarketDataPoint, boost::lockfree::capacity<1024>> price_queue_;

  // Thread management
  std::thread socket_reader_thread_;
  std::thread grpc_writer_thread_;

  // Atomic flag to signal threads to stop
  std::atomic<bool> should_stop_{false};
};