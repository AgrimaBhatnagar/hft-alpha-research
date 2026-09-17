#include "order_book.hpp"

#include <cassert>
#include <cmath>
#include <iostream>

using namespace hft;

bool close_enough(
    double a,
    double b,
    double tolerance = 1e-9
) {
    return std::abs(a - b) < tolerance;
}

int main() {

    OrderBook book;

    // Add bid
    book.process_event(
        MarketEvent(
            1,
            1,
            Side::BUY,
            EventType::ADD,
            100.0,
            100
        )
    );

    // Add ask
    book.process_event(
        MarketEvent(
            2,
            2,
            Side::SELL,
            EventType::ADD,
            100.10,
            200
        )
    );

    auto snapshot = book.snapshot();

    assert(
        close_enough(
            snapshot.best_bid,
            100.0
        )
    );

    assert(
        close_enough(
            snapshot.best_ask,
            100.10
        )
    );

    assert(
        close_enough(
            snapshot.mid_price,
            100.05
        )
    );

    assert(
        close_enough(
            snapshot.spread,
            0.10
        )
    );

    assert(
        snapshot.bid_depth == 100
    );

    assert(
        snapshot.ask_depth == 200
    );

    // OBI = (100 - 200) / (100 + 200)
    assert(
        close_enough(
            book.imbalance(1),
            -1.0 / 3.0
        )
    );

    // Cancel bid
    book.process_event(
        MarketEvent(
            3,
            1,
            Side::BUY,
            EventType::CANCEL,
            100.0,
            100
        )
    );

    assert(
        book.bid_levels() == 0
    );

    std::cout
        << "All OrderBook tests passed."
        << std::endl;

    return 0;
}