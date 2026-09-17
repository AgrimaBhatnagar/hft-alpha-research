#include "fast_order_book.hpp"

#include <chrono>
#include <cstdint>
#include <iomanip>
#include <iostream>
#include <vector>

using namespace hft;

int main() {

    constexpr size_t EVENT_COUNT = 5'000'000;

    std::vector<MarketEvent> events;
    events.reserve(EVENT_COUNT);

    uint64_t timestamp = 1'000'000'000;

    for (size_t i = 0; i < EVENT_COUNT; ++i) {

        const bool buy = (i & 1) == 0;

        EventType type;

        if (i % 10 == 0) {
            type = EventType::CANCEL;
        } else if (i % 7 == 0) {
            type = EventType::MODIFY;
        } else {
            type = EventType::ADD;
        }

        const double price =
            buy
                ? 100.00 - (i % 10) * 0.01
                : 100.01 + (i % 10) * 0.01;

        events.emplace_back(
            timestamp,
            i + 1,
            buy ? Side::BUY : Side::SELL,
            type,
            price,
            100 + static_cast<uint32_t>(i % 900)
        );

        timestamp += 1'000;
    }

    FastOrderBook book;

    for (size_t i = 0; i < 100'000; ++i) {
        book.process_event(events[i]);
    }

    book.clear();

    const auto start =
        std::chrono::steady_clock::now();

    for (const auto& event : events) {
        book.process_event(event);
    }

    const auto end =
        std::chrono::steady_clock::now();

    const double seconds =
        std::chrono::duration<double>(
            end - start
        ).count();

    const double throughput =
        static_cast<double>(EVENT_COUNT)
        / seconds;

    const double ns_per_event =
        seconds *
        1'000'000'000.0
        / static_cast<double>(EVENT_COUNT);

    std::cout
        << std::fixed
        << std::setprecision(2);

    std::cout
        << "\n============================================\n"
        << "      FAST ORDER BOOK BENCHMARK\n"
        << "============================================\n\n";

    std::cout
        << "Events processed:        "
        << EVENT_COUNT << '\n';

    std::cout
        << "Elapsed time:            "
        << seconds << " seconds\n";

    std::cout
        << "Throughput:              "
        << throughput << " events/sec\n";

    std::cout
        << "Average event time:      "
        << ns_per_event << " ns/event\n";

    std::cout
        << "Best bid:                "
        << book.best_bid() << '\n';

    std::cout
        << "Best ask:                "
        << book.best_ask() << '\n';

    std::cout
        << "Spread:                  "
        << book.spread() << '\n';

    std::cout
        << "OBI:                     "
        << book.imbalance(1) << '\n';

    std::cout
        << "\n============================================\n";

    return 0;
}