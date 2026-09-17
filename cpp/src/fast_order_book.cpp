#include "fast_order_book.hpp"

#include <algorithm>

namespace hft {

FastOrderBook::FastOrderBook() {
    clear();
}

void FastOrderBook::clear() {
    for (auto& level : bids_) {
        level = {};
    }

    for (auto& level : asks_) {
        level = {};
    }
}

double FastOrderBook::microprice() const
{
    const double bid = best_bid();
    const double ask = best_ask();

    const double bid_volume =
        static_cast<double>(total_bid_volume());

    const double ask_volume =
        static_cast<double>(total_ask_volume());

    const double total =
        bid_volume + ask_volume;

    if (total <= 0.0) {
        return mid_price();
    }

    return (
        ask * bid_volume +
        bid * ask_volume
    ) / total;
}
void FastOrderBook::process_event(
    const MarketEvent& event
) {
    switch (event.type) {

        case EventType::ADD:
            add(event.side, event.price, event.quantity);
            break;

        case EventType::MODIFY:
            modify(event.side, event.price, event.quantity);
            break;

        case EventType::CANCEL:
            cancel(event.side, event.price, event.quantity);
            break;

        case EventType::TRADE:
            cancel(event.side, event.price, event.quantity);
            break;
    }
}

void FastOrderBook::add(
    Side side,
    double price,
    uint32_t quantity
) {
    auto& levels =
        side == Side::BUY ? bids_ : asks_;

    const bool descending =
        side == Side::BUY;

    for (auto& level : levels) {

        if (level.active &&
            level.price == price) {

            level.quantity += quantity;
            return;
        }
    }

    insert_level(
        levels,
        price,
        quantity,
        descending
    );
}

void FastOrderBook::modify(
    Side side,
    double price,
    uint32_t quantity
) {
    auto& levels =
        side == Side::BUY ? bids_ : asks_;

    for (auto& level : levels) {

        if (level.active &&
            level.price == price) {

            level.quantity = quantity;
            return;
        }
    }
}

void FastOrderBook::cancel(
    Side side,
    double price,
    uint32_t quantity
) {
    auto& levels =
        side == Side::BUY ? bids_ : asks_;

    remove_quantity(
        levels,
        price,
        quantity
    );
}

void FastOrderBook::insert_level(
    std::array<Level, LEVELS>& levels,
    double price,
    uint32_t quantity,
    bool descending
) {
    std::size_t count = 0;

    while (
        count < LEVELS &&
        levels[count].active
    ) {
        ++count;
    }

    if (count == LEVELS) {
        return;
    }

    levels[count] = {
        price,
        quantity,
        true
    };

    sort_levels(
        levels,
        descending
    );
}

void FastOrderBook::remove_quantity(
    std::array<Level, LEVELS>& levels,
    double price,
    uint32_t quantity
) {
    for (auto& level : levels) {

        if (!level.active ||
            level.price != price) {
            continue;
        }

        if (level.quantity <= quantity) {
            level = {};
        } else {
            level.quantity -= quantity;
        }

        break;
    }

    // Compact active levels.
    std::size_t write = 0;

    for (std::size_t read = 0;
         read < LEVELS;
         ++read) {

        if (levels[read].active) {

            if (write != read) {
                levels[write] = levels[read];
                levels[read] = {};
            }

            ++write;
        }
    }
}

void FastOrderBook::sort_levels(
    std::array<Level, LEVELS>& levels,
    bool descending
) {
    std::sort(
        levels.begin(),
        levels.end(),
        [descending](const Level& a,
                     const Level& b) {

            if (!a.active) {
                return false;
            }

            if (!b.active) {
                return true;
            }

            if (descending) {
                return a.price > b.price;
            }

            return a.price < b.price;
        }
    );
}

double FastOrderBook::best_bid() const {
    return bids_[0].active
        ? bids_[0].price
        : 0.0;
}

double FastOrderBook::best_ask() const {
    return asks_[0].active
        ? asks_[0].price
        : 0.0;
}

double FastOrderBook::mid_price() const {
    if (best_bid() == 0.0 ||
        best_ask() == 0.0) {
        return 0.0;
    }

    return (best_bid() + best_ask()) * 0.5;
}

double FastOrderBook::spread() const {
    if (best_bid() == 0.0 ||
        best_ask() == 0.0) {
        return 0.0;
    }

    return best_ask() - best_bid();
}

uint64_t FastOrderBook::total_bid_volume() const {
    uint64_t total = 0;

    for (const auto& level : bids_) {
        if (level.active) {
            total += level.quantity;
        }
    }

    return total;
}

uint64_t FastOrderBook::total_ask_volume() const {
    uint64_t total = 0;

    for (const auto& level : asks_) {
        if (level.active) {
            total += level.quantity;
        }
    }

    return total;
}

double FastOrderBook::imbalance(
    std::size_t levels
) const {
    levels = std::min(levels, LEVELS);

    uint64_t bid = 0;
    uint64_t ask = 0;

    for (std::size_t i = 0; i < levels; ++i) {

        if (bids_[i].active) {
            bid += bids_[i].quantity;
        }

        if (asks_[i].active) {
            ask += asks_[i].quantity;
        }
    }

    const uint64_t total = bid + ask;

    if (total == 0) {
        return 0.0;
    }

    return static_cast<double>(
        static_cast<double>(bid) -
        static_cast<double>(ask)
    ) / static_cast<double>(total);
}

}