#include "python_api_gtw.h"

#include <boost/array.hpp>
#include <boost/asio.hpp>
#include <iostream>
#include <nlohmann/json.hpp>
#include <string>

using namespace boost::asio::ip;
using json = nlohmann::json;

PythonApiGtw::PythonApiGtw() {}

PythonApiGtw::~PythonApiGtw() {
  should_stop_.store(true);

  if (socket_reader_thread_.joinable()) {
    socket_reader_thread_.join();
  }

  {
    std::shared_lock<std::shared_mutex> lock(subscribers_mutex_);
    for (auto &sub : subscribers_) {
      sub->active.store(false);
    }
  }
}

void PythonApiGtw::Start() {
  if (running_.load()) {
    return;
  }
  should_stop_.store(false);
  socket_reader_thread_ = std::thread(&PythonApiGtw::SocketReaderThread, this);
}

std::shared_ptr<MarketDataSubscription> PythonApiGtw::Subscribe() {
  auto subscription = std::make_shared<MarketDataSubscription>();

  {
    std::unique_lock<std::shared_mutex> lock(subscribers_mutex_);
    subscribers_.push_back(subscription);
  }

  std::cout << "New client subscribed (total: " << subscribers_.size() << ")"
            << std::endl;
  return subscription;
}

void PythonApiGtw::Unsubscribe(
    const std::shared_ptr<MarketDataSubscription> &subscription) {
  std::unique_lock<std::shared_mutex> lock(subscribers_mutex_);

  auto it = std::find(subscribers_.begin(), subscribers_.end(), subscription);
  if (it != subscribers_.end()) {
    subscribers_.erase(it);
  }

  std::cout << "Client unsubscribed (remaining: " << subscribers_.size() << ")"
            << std::endl;
}

bool PythonApiGtw::IsRunning() const { return running_.load(); }

void PythonApiGtw::Broadcast(const MarketDataPoint &point) {
  std::shared_lock<std::shared_mutex> lock(subscribers_mutex_);

  for (auto &sub : subscribers_) {
    if (!sub->queue.push(point)) {
      std::cerr << "WARNING: Queue full for a subscriber, dropping update for "
                << point.instrument_id << std::endl;
    }
  }
}

void PythonApiGtw::SocketReaderThread() {
  running_.store(true);

  try {
    boost::asio::io_service ios;

    tcp::endpoint endpoint(boost::asio::ip::address::from_string("127.0.0.1"),
                           9000);
    tcp::socket socket(ios);

    socket.connect(endpoint);
    std::cout << "Connected to Python API Gateway on port 9000" << std::endl;

    boost::array<char, 4096> buffer;
    std::string accumulated_data;
    constexpr size_t max_accumulated_size = 1024 * 1024;

    while (!should_stop_.load()) {
      boost::system::error_code error;

      size_t len = socket.read_some(boost::asio::buffer(buffer), error);

      if (error == boost::asio::error::eof) {
        std::cout << "Connection closed by peer" << std::endl;
        break;
      } else if (error) {
        std::cerr << "Error reading from socket: " << error.message()
                  << std::endl;
        break;
      }

      accumulated_data.append(buffer.data(), len);

      if (accumulated_data.size() > max_accumulated_size) {
        std::cerr << "Accumulated data exceeded limit, clearing buffer"
                  << std::endl;
        accumulated_data.clear();
        continue;
      }

      size_t newline_pos;
      while ((newline_pos = accumulated_data.find('\n')) != std::string::npos) {
        std::string json_line = accumulated_data.substr(0, newline_pos);
        accumulated_data.erase(0, newline_pos + 1);

        if (json_line.empty()) {
          continue;
        }

        try {
          json json_data = json::parse(json_line);

          MarketDataPoint data_point;

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

          Broadcast(data_point);

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

  running_.store(false);

  {
    std::shared_lock<std::shared_mutex> lock(subscribers_mutex_);
    for (auto &sub : subscribers_) {
      sub->active.store(false);
    }
  }

  std::cout << "Socket reader thread exiting" << std::endl;
}
