#include "fast_order_book.hpp"

#include <cassert>
#include <cmath>
#include <iostream>

using namespace hft;

static bool close_enough(
    double a,
    double b,
    double epsilon = 1e-9
) {
    return std::abs(a - b) < epsilon;
}

int main() {

    FastOrderBook book;

    // Empty book
    assert(close_enough(book.best_bid(), 0.0));
    assert(close_enough(book.best_ask(), 0.0));

    // Add bid
    book.process_event(
        MarketEvent(
            1'000,
            1,
            Side::BUY,
            EventType::ADD,
            100.00,
            500
        )
    );

    // Add ask
    book.process_event(
        MarketEvent(
            2'000,
            2,
            Side::SELL,
            EventType::ADD,
            100.01,
            300
        )
    );

    assert(close_enough(
        book.best_bid(),
        100.00
    ));

    assert(close_enough(
        book.best_ask(),
        100.01
    ));

    assert(close_enough(
        book.spread(),
        0.01
    ));

    assert(
        book.total_bid_volume() == 500
    );

    assert(
        book.total_ask_volume() == 300
    );

    // Add another bid level
    book.process_event(
        MarketEvent(
            3'000,
            3,
            Side::BUY,
            EventType::ADD,
            99.99,
            700
        )
    );

    // Add another ask level
    book.process_event(
        MarketEvent(
            4'000,
            4,
            Side::SELL,
            EventType::ADD,
            100.02,
            800
        )
    );

    assert(
        book.total_bid_volume() == 1200
    );

    assert(
        book.total_ask_volume() == 1100
    );

    // Modify
    book.process_event(
        MarketEvent(
            5'000,
            3,
            Side::BUY,
            EventType::MODIFY,
            99.99,
            400
        )
    );

    assert(
        book.total_bid_volume() == 900
    );

    // Cancel
    book.process_event(
        MarketEvent(
            6'000,
            2,
            Side::SELL,
            EventType::CANCEL,
            100.01,
            100
        )
    );

    assert(
        book.total_ask_volume() == 1000
    );

    // Best levels should remain unchanged
    assert(close_enough(
        book.best_bid(),
        100.00
    ));

    assert(close_enough(
        book.best_ask(),
        100.01
    ));

    // OBI should be valid
    const double obi =
        book.imbalance(1);

    assert(
        obi >= -1.0 &&
        obi <= 1.0
    );

    std::cout
        << "FastOrderBook tests passed.\n";

    return 0;
}