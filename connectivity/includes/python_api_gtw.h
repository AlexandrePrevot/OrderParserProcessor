#pragma once

#include <atomic>
#include <memory>
#include <shared_mutex>
#include <thread>
#include <vector>

#include "market_data_subscription.h"

class PythonApiGtw {
public:
    PythonApiGtw();
    ~PythonApiGtw();

    PythonApiGtw(const PythonApiGtw&) = delete;
    PythonApiGtw& operator=(const PythonApiGtw&) = delete;

    void Start();

    std::shared_ptr<MarketDataSubscription> Subscribe();
    void Unsubscribe(const std::shared_ptr<MarketDataSubscription>& subscription);

    bool IsRunning() const;

private:
    void SocketReaderThread();
    void Broadcast(const MarketDataPoint& point);

    std::atomic<bool> should_stop_{false};
    std::atomic<bool> running_{false};
    std::thread socket_reader_thread_;

    std::shared_mutex subscribers_mutex_;
    std::vector<std::shared_ptr<MarketDataSubscription>> subscribers_;
};
