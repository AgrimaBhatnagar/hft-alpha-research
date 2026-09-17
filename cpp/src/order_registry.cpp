#include "order_registry.hpp"

namespace hft {

bool OrderRegistry::add(
    const MarketEvent& event
) {
    orders_[event.order_id] = {
        event.side,
        event.price,
        event.quantity
    };

    return true;
}


bool OrderRegistry::reduce(
    uint64_t order_id,
    uint32_t quantity
) {
    auto it = orders_.find(order_id);

    if (it == orders_.end()) {
        return false;
    }

    if (quantity >= it->second.quantity) {

        orders_.erase(it);

    } else {

        it->second.quantity -= quantity;
    }

    return true;
}


bool OrderRegistry::remove(
    uint64_t order_id
) {
    return orders_.erase(order_id) > 0;
}


bool OrderRegistry::get(
    uint64_t order_id,
    OrderRecord& record
) const {
    auto it = orders_.find(order_id);

    if (it == orders_.end()) {
        return false;
    }

    record = it->second;

    return true;
}


void OrderRegistry::clear() {
    orders_.clear();
}


std::size_t OrderRegistry::size() const {
    return orders_.size();
}

}