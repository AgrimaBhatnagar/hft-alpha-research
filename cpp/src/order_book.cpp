#include "order_book.hpp"

#include <algorithm>
#include <cmath>

namespace hft {

OrderBook::OrderBook(size_t depth)
    : depth_(depth) {}


void OrderBook::process_event(
    const MarketEvent& event
) {

    switch (event.type) {

        case EventType::ADD:
            add_order(event);
            break;

        case EventType::MODIFY:
            modify_order(event);
            break;

        case EventType::CANCEL:
            cancel_order(event);
            break;

        case EventType::TRADE:
            cancel_order(event);
            break;
    }
}


void OrderBook::add_order(
    const MarketEvent& event
) {

    auto existing = orders_.find(event.order_id);

    if (existing != orders_.end()) {
        cancel_order(event);
    }

    Order order {
        event.order_id,
        event.price,
        event.quantity,
        event.side
    };

    orders_[event.order_id] = order;

    if (event.side == Side::BUY) {

        bids_[event.price] += event.quantity;

    } else {

        asks_[event.price] += event.quantity;
    }
}


void OrderBook::modify_order(
    const MarketEvent& event
) {

    auto it = orders_.find(event.order_id);

    if (it == orders_.end()) {

        add_order(event);

        return;
    }

    Order& order = it->second;

    remove_quantity(
        order.side,
        order.price,
        order.quantity
    );

    order.price = event.price;

    order.quantity = event.quantity;

    if (order.side == Side::BUY) {

        bids_[order.price] += order.quantity;

    } else {

        asks_[order.price] += order.quantity;
    }
}


void OrderBook::cancel_order(
    const MarketEvent& event
) {

    auto it = orders_.find(event.order_id);

    if (it == orders_.end()) {
        return;
    }

    const Order& order = it->second;

    remove_quantity(
        order.side,
        order.price,
        order.quantity
    );

    orders_.erase(it);
}


void OrderBook::remove_quantity(
    Side side,
    double price,
    uint32_t quantity
) {

    if (side == Side::BUY) {

        auto it = bids_.find(price);

        if (it == bids_.end()) {
            return;
        }

        if (it->second <= quantity) {

            bids_.erase(it);

        } else {

            it->second -= quantity;
        }

    } else {

        auto it = asks_.find(price);

        if (it == asks_.end()) {
            return;
        }

        if (it->second <= quantity) {

            asks_.erase(it);

        } else {

            it->second -= quantity;
        }
    }
}


std::optional<PriceLevel>
OrderBook::best_bid() const {

    if (bids_.empty()) {
        return std::nullopt;
    }

    auto it = bids_.begin();

    return PriceLevel {
        it->first,
        it->second,
        1
    };
}


std::optional<PriceLevel>
OrderBook::best_ask() const {

    if (asks_.empty()) {
        return std::nullopt;
    }

    auto it = asks_.begin();

    return PriceLevel {
        it->first,
        it->second,
        1
    };
}


BookSnapshot OrderBook::snapshot() const {

    BookSnapshot result;

    auto bid = best_bid();

    auto ask = best_ask();

    if (!bid || !ask) {
        return result;
    }

    result.best_bid = bid->price;

    result.best_ask = ask->price;

    result.mid_price =
        (result.best_bid + result.best_ask)
        / 2.0;

    result.spread =
        result.best_ask - result.best_bid;

    result.bid_depth =
        total_bid_volume();

    result.ask_depth =
        total_ask_volume();

    return result;
}


double OrderBook::microprice() const {

    auto bid = best_bid();

    auto ask = best_ask();

    if (!bid || !ask) {
        return 0.0;
    }

    const double bid_volume =
        static_cast<double>(bid->quantity);

    const double ask_volume =
        static_cast<double>(ask->quantity);

    const double total =
        bid_volume + ask_volume;

    if (total == 0.0) {
        return (bid->price + ask->price) / 2.0;
    }

    return (
        ask->price * bid_volume
        +
        bid->price * ask_volume
    ) / total;
}


double OrderBook::imbalance(
    size_t levels
) const {

    const uint64_t bid_volume =
        depth_volume(
            Side::BUY,
            levels
        );

    const uint64_t ask_volume =
        depth_volume(
            Side::SELL,
            levels
        );

    const double total =
        static_cast<double>(
            bid_volume + ask_volume
        );

    if (total == 0.0) {
        return 0.0;
    }

    return (
        static_cast<double>(bid_volume)
        -
        static_cast<double>(ask_volume)
    ) / total;
}


uint64_t OrderBook::depth_volume(
    Side side,
    size_t levels
) const {

    uint64_t total = 0;

    size_t count = 0;

    if (side == Side::BUY) {

        for (
            const auto& [price, quantity]
            : bids_
        ) {

            total += quantity;

            ++count;

            if (count >= levels) {
                break;
            }
        }

    } else {

        for (
            const auto& [price, quantity]
            : asks_
        ) {

            total += quantity;

            ++count;

            if (count >= levels) {
                break;
            }
        }
    }

    return total;
}


size_t OrderBook::bid_levels() const {
    return bids_.size();
}


size_t OrderBook::ask_levels() const {
    return asks_.size();
}


uint64_t OrderBook::total_bid_volume() const {

    uint64_t total = 0;

    for (const auto& [price, quantity] : bids_) {
        total += quantity;
    }

    return total;
}


uint64_t OrderBook::total_ask_volume() const {

    uint64_t total = 0;

    for (const auto& [price, quantity] : asks_) {
        total += quantity;
    }

    return total;
}


void OrderBook::clear() {

    bids_.clear();

    asks_.clear();

    orders_.clear();
}

}