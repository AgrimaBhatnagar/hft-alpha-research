#include "order_aware_book.hpp"

#include <algorithm>

namespace hft {

OrderAwareBook::OrderAwareBook() {
    clear();
}

void OrderAwareBook::clear() {

    orders_.clear();
    bids_.clear();
    asks_.clear();
}


void OrderAwareBook::add_level(
    Side side,
    double price,
    uint32_t quantity
) {

    if (side == Side::BUY) {
        bids_[price] += quantity;
    }
    else {
        asks_[price] += quantity;
    }
}


void OrderAwareBook::remove_level(
    Side side,
    double price,
    uint32_t quantity
) {

    auto& book =
        (side == Side::BUY)
        ? bids_
        : asks_;

    auto it = book.find(price);

    if (it == book.end())
        return;

    if (it->second <= quantity) {
        book.erase(it);
    }
    else {
        it->second -= quantity;
    }
}


void OrderAwareBook::add_order(
    const MarketEvent& event
) {

    if (event.order_id == 0)
        return;

    auto existing =
        orders_.find(event.order_id);

    if (existing != orders_.end()) {
        remove_order(event.order_id);
    }

    Order order{
        event.order_id,
        event.side,
        event.price,
        event.quantity
    };

    orders_[event.order_id] = order;

    add_level(
        event.side,
        event.price,
        event.quantity
    );
}


void OrderAwareBook::remove_order(
    uint64_t order_id
) {

    auto it =
        orders_.find(order_id);

    if (it == orders_.end())
        return;

    const Order& order = it->second;

    remove_level(
        order.side,
        order.price,
        order.quantity
    );

    orders_.erase(it);
}


void OrderAwareBook::reduce_order(
    uint64_t order_id,
    uint32_t quantity
) {

    auto it =
        orders_.find(order_id);

    if (it == orders_.end())
        return;

    Order& order = it->second;

    const uint32_t reduction =
        std::min(
            quantity,
            order.quantity
        );

    remove_level(
        order.side,
        order.price,
        reduction
    );

    order.quantity -= reduction;

    if (order.quantity == 0) {
        orders_.erase(it);
    }
}


void OrderAwareBook::process_event(
    const MarketEvent& event
) {

    switch (event.type) {

        case EventType::ADD:

            add_order(event);
            break;

        case EventType::MODIFY:

            reduce_order(
                event.order_id,
                event.quantity
            );
            break;

        case EventType::CANCEL:

            remove_order(
                event.order_id
            );
            break;

        case EventType::TRADE:

            reduce_order(
                event.order_id,
                event.quantity
            );
            break;
    }
}


double OrderAwareBook::best_bid() const {

    if (bids_.empty())
        return 0.0;

    return bids_.begin()->first;
}


double OrderAwareBook::best_ask() const {

    if (asks_.empty())
        return 0.0;

    return asks_.begin()->first;
}


double OrderAwareBook::mid_price() const {

    const double bid = best_bid();
    const double ask = best_ask();

    if (bid <= 0.0 || ask <= 0.0)
        return 0.0;

    return (bid + ask) * 0.5;
}


double OrderAwareBook::spread() const {

    const double bid = best_bid();
    const double ask = best_ask();

    if (bid <= 0.0 || ask <= 0.0)
        return 0.0;

    return ask - bid;
}


uint64_t OrderAwareBook::bid_volume() const {

    uint64_t total = 0;

    for (const auto& level : bids_)
        total += level.second;

    return total;
}


uint64_t OrderAwareBook::ask_volume() const {

    uint64_t total = 0;

    for (const auto& level : asks_)
        total += level.second;

    return total;
}


double OrderAwareBook::imbalance() const {

    const double bid =
        static_cast<double>(bid_volume());

    const double ask =
        static_cast<double>(ask_volume());

    const double total = bid + ask;

    if (total <= 0.0)
        return 0.0;

    return (bid - ask) / total;
}

}