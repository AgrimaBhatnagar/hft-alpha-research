#pragma once

#include "market_event.hpp"

#include <cstdint>
#include <unordered_map>

namespace hft {

struct OrderRecord {
    Side side;
    double price;
    uint32_t quantity;
};

class OrderRegistry {
public:

    bool add(const MarketEvent& event);

    bool reduce(
        uint64_t order_id,
        uint32_t quantity
    );

    bool remove(
        uint64_t order_id
    );

    bool get(
        uint64_t order_id,
        OrderRecord& record
    ) const;

    void clear();

    std::size_t size() const;

private:

    std::unordered_map<
        uint64_t,
        OrderRecord
    > orders_;
};

}