#pragma once

#include <cstdint>

namespace hft {

enum class Side : uint8_t {
    BUY = 0,
    SELL = 1
};

enum class EventType : uint8_t {
    ADD = 0,
    MODIFY = 1,
    CANCEL = 2,
    TRADE = 3
};

struct MarketEvent {

    uint64_t timestamp;

    uint64_t order_id;

    Side side;

    EventType type;

    double price;

    uint32_t quantity;

    MarketEvent(
        uint64_t timestamp_,
        uint64_t order_id_,
        Side side_,
        EventType type_,
        double price_,
        uint32_t quantity_
    )
        : timestamp(timestamp_),
          order_id(order_id_),
          side(side_),
          type(type_),
          price(price_),
          quantity(quantity_) {}
};

}