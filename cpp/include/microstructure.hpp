#pragma once

#include "order_book.hpp"

namespace hft {

class MicrostructureEngine {

public:

    static double spread(
        const OrderBook& book
    );

    static double relative_spread(
        const OrderBook& book
    );

    static double mid_price(
        const OrderBook& book
    );

    static double microprice(
        const OrderBook& book
    );

    static double imbalance(
        const OrderBook& book,
        size_t levels = 1
    );

    static double microprice_deviation(
        const OrderBook& book
    );
};

}