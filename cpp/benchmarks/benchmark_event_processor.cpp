#include "event_processor.hpp"

#include <chrono>
#include <cstdint>
#include <iostream>
#include <vector>

using namespace hft;

int main() {

    constexpr size_t EVENT_COUNT = 1'000'000;

    std::vector<MarketEvent> events;
    events.reserve(EVENT_COUNT);

    uint64_t timestamp = 1'000'000'000;

    // Generate a deterministic synthetic market-data stream.
    for (size_t i = 0; i < EVENT_COUNT; ++i) {

        const bool is_buy = (i % 2 == 0);

        const EventType type =
            (i % 10 == 0)
                ? EventType::CANCEL
                : (i % 7 == 0)
                    ? EventType::MODIFY
                    : EventType::ADD;

        const double price =
            is_buy
                ? 100.00 - static_cast<double>(i % 10) * 0.01
                : 100.01 + static_cast<double>(i % 10) * 0.01;

        const uint32_t quantity =
            100 + static_cast<uint32_t>(i % 900);

        events.emplace_back(
            timestamp,
            i + 1,
            is_buy ? Side::BUY : Side::SELL,
            type,
            price,
            quantity
        );

        timestamp += 1'000;
    }

    EventProcessor processor(100);

    const auto start =
        std::chrono::high_resolution_clock::now();

    processor.process_batch(events);

    const auto end =
        std::chrono::high_resolution_clock::now();

    const std::chrono::duration<double> elapsed =
        end - start;

    const double seconds =
        elapsed.count();

    const double events_per_second =
        static_cast<double>(EVENT_COUNT)
        /
        seconds;

    const double nanoseconds_per_event =
        seconds * 1'000'000'000.0
        /
        static_cast<double>(EVENT_COUNT);

    const auto& stats =
        processor.statistics();

    std::cout
        << "\n============================================\n"
        << "        HFT EVENT PROCESSOR BENCHMARK\n"
        << "============================================\n\n";

    std::cout
        << "Events processed:        "
        << EVENT_COUNT
        << '\n';

    std::cout
        << "Elapsed time:            "
        << seconds
        << " seconds\n";

    std::cout
        << "Events / second:         "
        << events_per_second
        << '\n';

    std::cout
        << "Nanoseconds / event:     "
        << nanoseconds_per_event
        << '\n';

    std::cout
        << "Total volume:            "
        << stats.total_volume
        << '\n';

    std::cout
        << "Global OFI:              "
        << stats.order_flow_imbalance
        << '\n';

    std::cout
        << "\nFinal mid price:         "
        << processor.order_book()
               .snapshot()
               .mid_price
        << '\n';

    std::cout
        << "Final spread:            "
        << processor.order_book()
               .snapshot()
               .spread
        << '\n';

    std::cout
        << "\n============================================\n";

    return 0;
}