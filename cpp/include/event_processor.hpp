#pragma once

#include "market_event.hpp"
#include "order_book.hpp"

#include <cstdint>
#include <deque>
#include <vector>

namespace hft {

struct EventStatistics {

    uint64_t total_events = 0;

    uint64_t add_events = 0;

    uint64_t modify_events = 0;

    uint64_t cancel_events = 0;

    uint64_t trade_events = 0;

    uint64_t buy_events = 0;

    uint64_t sell_events = 0;

    uint64_t buy_volume = 0;

    uint64_t sell_volume = 0;

    uint64_t total_volume = 0;

    double signed_volume = 0.0;

    double order_flow_imbalance = 0.0;

    double trade_intensity = 0.0;
};


class EventProcessor {

public:

    explicit EventProcessor(
        size_t rolling_window = 100
    );

    void process(
        const MarketEvent& event
    );

    void process_batch(
        const std::vector<MarketEvent>& events
    );

    const OrderBook& order_book() const;

    const EventStatistics& statistics() const;

    double rolling_signed_volume() const;

    double rolling_volume_imbalance() const;

    double events_per_second() const;

    void reset();

private:

    OrderBook order_book_;

    EventStatistics statistics_;

    size_t rolling_window_;

    std::deque<MarketEvent> recent_events_;

    uint64_t first_timestamp_ = 0;

    uint64_t last_timestamp_ = 0;

    uint64_t rolling_buy_volume_ = 0;

    uint64_t rolling_sell_volume_ = 0;

    void update_statistics(
        const MarketEvent& event
    );

    void update_rolling_window(
        const MarketEvent& event
    );
};

}