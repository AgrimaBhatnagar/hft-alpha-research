#pragma once

#include "market_event.hpp"

#include <map>
#include <unordered_map>
#include <cstdint>
#include <vector>
#include <optional>

namespace hft {

struct PriceLevel {

    double price;

    uint64_t quantity;

    uint64_t order_count;
};

struct BookSnapshot {

    double best_bid = 0.0;

    double best_ask = 0.0;

    double mid_price = 0.0;

    double spread = 0.0;

    uint64_t bid_depth = 0;

    uint64_t ask_depth = 0;
};

class OrderBook {

public:

    explicit OrderBook(size_t depth = 10);

    void process_event(const MarketEvent& event);

    std::optional<PriceLevel> best_bid() const;

    std::optional<PriceLevel> best_ask() const;

    BookSnapshot snapshot() const;

    double microprice() const;

    double imbalance(size_t levels = 1) const;

    size_t bid_levels() const;

    size_t ask_levels() const;

    uint64_t total_bid_volume() const;

    uint64_t total_ask_volume() const;

    void clear();

private:

    struct Order {

        uint64_t order_id;

        double price;

        uint32_t quantity;

        Side side;
    };

    size_t depth_;

    // Highest bid first
    std::map<double, uint64_t, std::greater<double>> bids_;

    // Lowest ask first
    std::map<double, uint64_t, std::less<double>> asks_;

    std::unordered_map<uint64_t, Order> orders_;

    void add_order(const MarketEvent& event);

    void modify_order(const MarketEvent& event);

    void cancel_order(const MarketEvent& event);

    void remove_quantity(
        Side side,
        double price,
        uint32_t quantity
    );

    uint64_t depth_volume(
        Side side,
        size_t levels
    ) const;
};

}