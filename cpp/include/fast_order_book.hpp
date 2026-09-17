#pragma once

#include "market_event.hpp"

#include <array>
#include <cstddef>
#include <cstdint>

namespace hft {

class FastOrderBook {
public:
    static constexpr std::size_t LEVELS = 64;

    FastOrderBook();

    void process_event(const MarketEvent& event);

    double best_bid() const;
    double best_ask() const;
    double mid_price() const;
    double spread() const;
    double microprice() const;

    uint64_t total_bid_volume() const;
    uint64_t total_ask_volume() const;

    double imbalance(std::size_t levels = 1) const;

    void clear();

private:
    struct Level {
        double price = 0.0;
        uint64_t quantity = 0;
        bool active = false;
    };

    std::array<Level, LEVELS> bids_;
    std::array<Level, LEVELS> asks_;

    void add(
        Side side,
        double price,
        uint32_t quantity
    );

    void modify(
        Side side,
        double price,
        uint32_t quantity
    );

    void cancel(
        Side side,
        double price,
        uint32_t quantity
    );

    static void insert_level(
        std::array<Level, LEVELS>& levels,
        double price,
        uint32_t quantity,
        bool descending
    );

    static void remove_quantity(
        std::array<Level, LEVELS>& levels,
        double price,
        uint32_t quantity
    );

    static void sort_levels(
        std::array<Level, LEVELS>& levels,
        bool descending
    );
};

}