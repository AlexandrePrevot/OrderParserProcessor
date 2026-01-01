#pragma once

#include <cstdint>
#include <cstring>

// Plain Old Data struct for passing market data between threads
// Must be trivially copyable for use with boost::lockfree::spsc_queue
// It is added because we want to pass a simple data structure to the queue
// so that pushing to the queue is fast
struct MarketDataPoint {
    double price;
    int64_t quantity;
    int64_t timestamp_seconds;
    int32_t timestamp_nanos;
    char instrument_id[32];  // Fixed size for trivial copyability

    // Default constructor
    MarketDataPoint()
        : price(0.0)
        , quantity(0)
        , timestamp_seconds(0)
        , timestamp_nanos(0)
        , instrument_id{0} {}

    // Helper to set instrument_id safely
    void set_instrument_id(const char* id) {
        std::strncpy(instrument_id, id, sizeof(instrument_id) - 1);
        instrument_id[sizeof(instrument_id) - 1] = '\0';  // Ensure null termination
    }
};

static_assert(std::is_trivially_copyable<MarketDataPoint>::value,
              "MarketDataPoint must be trivially copyable for lock-free queue");
