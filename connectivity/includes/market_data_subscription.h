#pragma once

#include <atomic>
#include <boost/lockfree/spsc_queue.hpp>
#include "market_data_point.h"

struct MarketDataSubscription {
    using Queue = boost::lockfree::spsc_queue<MarketDataPoint, boost::lockfree::capacity<1024>>;

    Queue queue;
    std::atomic<bool> active{true};
};
