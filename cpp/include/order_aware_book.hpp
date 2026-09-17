#pragma once

#include "market_event.hpp"

#include <cstdint>
#include <map>
#include <unordered_map>

namespace hft {

class OrderAwareBook {
public:

    OrderAwareBook();

    void process_event(const MarketEvent& event);

    double best_bid() const;
    double best_ask() const;

    double mid_price() const;
    double spread() const;

    uint64_t bid_volume() const;
    uint64_t ask_volume() const;

    double imbalance() const;

    void clear();

private:

    struct Order {
        uint64_t id;
        Side side;
        double price;
        uint32_t quantity;
    };

    std::unordered_map<uint64_t, Order> orders_;

    std::map<double, uint64_t, std::greater<double>> bids_;
    std::map<double, uint64_t, std::less<double>> asks_;

    void add_order(const MarketEvent& event);

    void reduce_order(
        uint64_t order_id,
        uint32_t quantity
    );

    void remove_order(
        uint64_t order_id
    );

    void add_level(
        Side side,
        double price,
        uint32_t quantity
    );

    void remove_level(
        Side side,
        double price,
        uint32_t quantity
    );
};

}