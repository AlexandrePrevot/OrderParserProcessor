#include "python_api_gtw.h"

#include <boost/array.hpp>
#include <boost/asio.hpp>
#include <chrono>
#include <google/protobuf/timestamp.pb.h>
#include <grpcpp/server_context.h>
#include <grpcpp/support/sync_stream.h>
#include <iostream>
#include <nlohmann/json.hpp>
#include <string>

using namespace boost::asio::ip;
using json = nlohmann::json;

PythonApiGtw::PythonApiGtw() : active_(false) {}

PythonApiGtw::~PythonApiGtw() {
  // Signal threads to stop
  should_stop_.store(true);

  // Join threads if they're joinable
  if (socket_reader_thread_.joinable()) {
    socket_reader_thread_.join();
  }
  if (grpc_writer_thread_.joinable()) {
    grpc_writer_thread_.join();
  }

  // No cleanup needed - POD structs in queue are just discarded
}

void PythonApiGtw::Activate() { active_ = true; }

bool PythonApiGtw::ConnectToGtw(
    grpc::ServerWriter<internal::PriceUpdate> *writer) {
  if (!writer) {
    std::cerr << "Error: gRPC writer is null" << std::endl;
    return false;
  }

  // Reset stop flag
  should_stop_.store(false);

  // Start gRPC writer thread first
  grpc_writer_thread_ =
      std::thread(&PythonApiGtw::GrpcWriterThread, this, writer);

  // Start socket reader thread
  socket_reader_thread_ = std::thread(&PythonApiGtw::SocketReaderThread, this);

  // Wait for both threads to complete
  if (socket_reader_thread_.joinable()) {
    socket_reader_thread_.join();
  }

  // Signal writer to stop and wait
  should_stop_.store(true);
  if (grpc_writer_thread_.joinable()) {
    grpc_writer_thread_.join();
  }

  return true;
}

void PythonApiGtw::SocketReaderThread() {
  try {
    // Create ASIO service
    boost::asio::io_service ios;

    // Connect to Python gateway on localhost:9000
    tcp::endpoint endpoint(boost::asio::ip::address::from_string("127.0.0.1"),
                           9000);
    tcp::socket socket(ios);

    // Connect to the socket
    socket.connect(endpoint);
    std::cout << "Connected to Python API Gateway on port 9000" << std::endl;

    // Buffer for receiving data
    boost::array<char, 4096> buffer;
    std::string accumulated_data;
    constexpr size_t max_accumulated_size = 1024 * 1024; // 1MB limit

    while (!should_stop_.load()) {
      boost::system::error_code error;

      // synchronous function to get the data (see boost asio documentation)
      size_t len = socket.read_some(boost::asio::buffer(buffer), error);

      if (error == boost::asio::error::eof) {
        std::cout << "Connection closed by peer" << std::endl;
        break;
      } else if (error) {
        std::cerr << "Error reading from socket: " << error.message()
                  << std::endl;
        break;
      }

      // Accumulate received data
      accumulated_data.append(buffer.data(), len);

      // Safety check: prevent unbounded growth
      if (accumulated_data.size() > max_accumulated_size) {
        std::cerr << "Accumulated data exceeded limit, clearing buffer"
                  << std::endl;
        accumulated_data.clear();
        continue;
      }

      // Process complete JSON messages (assume newline-delimited JSON)
      size_t newline_pos;
      while ((newline_pos = accumulated_data.find('\n')) != std::string::npos) {
        std::string json_line = accumulated_data.substr(0, newline_pos);
        accumulated_data.erase(0, newline_pos + 1);

        // Skip empty lines
        if (json_line.empty()) {
          continue;
        }

        try {
          // Parse JSON
          json json_data = json::parse(json_line);

          // Create POD struct on stack - no allocation!
          MarketDataPoint data_point;

          // Populate fields from JSON
          if (json_data.contains("price")) {
            data_point.price = json_data["price"].get<double>();
          }
          if (json_data.contains("quantity")) {
            data_point.quantity = json_data["quantity"].get<int64_t>();
          }
          if (json_data.contains("instrument_id")) {
            data_point.set_instrument_id(
                json_data["instrument_id"].get<std::string>().c_str());
          }

          // Set timestamp to current time
          auto now = std::chrono::system_clock::now();
          auto duration = now.time_since_epoch();
          auto seconds =
              std::chrono::duration_cast<std::chrono::seconds>(duration);
          auto nanos_total =
              std::chrono::duration_cast<std::chrono::nanoseconds>(duration);
          auto nanos_remainder =
              nanos_total -
              std::chrono::duration_cast<std::chrono::nanoseconds>(seconds);

          data_point.timestamp_seconds = seconds.count();
          data_point.timestamp_nanos =
              static_cast<int32_t>(nanos_remainder.count());

          // Try to push to lock-free queue - if full, drop this message
          if (!price_queue_.push(data_point)) {
            std::cerr << "WARNING: Queue full, dropping price update for "
                      << data_point.instrument_id << std::endl;
            // Message is dropped - no cleanup needed (POD on stack)
          } else {
            std::cout << "Queued price update: " << data_point.instrument_id
                      << " @ " << data_point.price << std::endl;
          }

        } catch (const json::parse_error &parse_error) {
          std::cerr << "JSON parse error: " << parse_error.what() << std::endl;
          std::cerr << "Raw data: " << json_line << std::endl;
        } catch (const std::exception &exception) {
          std::cerr << "Error processing message: " << exception.what()
                    << std::endl;
        }
      }
    }

  } catch (const std::exception &exception) {
    std::cerr << "Exception in SocketReaderThread: " << exception.what()
              << std::endl;
  }

  std::cout << "Socket reader thread exiting" << std::endl;
}

void PythonApiGtw::GrpcWriterThread(
    grpc::ServerWriter<internal::PriceUpdate> *writer) {
  try {
    std::cout << "gRPC writer thread started" << std::endl;

    while (!should_stop_.load()) {
      MarketDataPoint data_point;

      // Try to pop from queue
      if (price_queue_.pop(data_point)) {
        // Convert POD struct to protobuf message
        internal::PriceUpdate price_update;
        price_update.set_price(data_point.price);
        price_update.set_quantity(data_point.quantity);
        price_update.set_instrument_id(data_point.instrument_id);

        // Set timestamp
        auto *timestamp = price_update.mutable_timestamp();
        timestamp->set_seconds(data_point.timestamp_seconds);
        timestamp->set_nanos(data_point.timestamp_nanos);

        // Send via gRPC stream
        if (!writer->Write(price_update)) {
          std::cerr << "Failed to write to gRPC stream (client disconnected)"
                    << std::endl;
          break;
        }

        std::cout << "Sent PriceUpdate: " << data_point.instrument_id << " @ "
                  << data_point.price << std::endl;

      } else {
        // Queue is empty, sleep briefly to avoid busy-waiting
        std::this_thread::sleep_for(std::chrono::microseconds(100));
      }
    }

    // Drain remaining messages from queue before exiting
    MarketDataPoint data_point;
    while (price_queue_.pop(data_point)) {
      internal::PriceUpdate price_update;
      price_update.set_price(data_point.price);
      price_update.set_quantity(data_point.quantity);
      price_update.set_instrument_id(data_point.instrument_id);

      auto *timestamp = price_update.mutable_timestamp();
      timestamp->set_seconds(data_point.timestamp_seconds);
      timestamp->set_nanos(data_point.timestamp_nanos);

      if (writer->Write(price_update)) {
        std::cout << "Drained PriceUpdate: " << data_point.instrument_id
                  << std::endl;
      }
    }

  } catch (const std::exception &exception) {
    std::cerr << "Exception in GrpcWriterThread: " << exception.what()
              << std::endl;
  }

  std::cout << "gRPC writer thread exiting" << std::endl;
}
