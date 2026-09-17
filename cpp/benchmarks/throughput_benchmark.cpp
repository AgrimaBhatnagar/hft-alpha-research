#include "event_processor.hpp"

#include <chrono>
#include <cstdint>
#include <iomanip>
#include <iostream>
#include <vector>

using namespace hft;

int main() {

    constexpr size_t EVENT_COUNT = 5'000'000;
    constexpr size_t WARMUP_COUNT = 100'000;

    std::vector<MarketEvent> events;
    events.reserve(EVENT_COUNT);

    uint64_t timestamp = 1'000'000'000;

    for (size_t i = 0; i < EVENT_COUNT; ++i) {

        const bool is_buy = (i & 1) == 0;

        EventType type;

        if (i % 10 == 0) {
            type = EventType::CANCEL;
        } else if (i % 7 == 0) {
            type = EventType::MODIFY;
        } else {
            type = EventType::ADD;
        }

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

    EventProcessor warmup_processor(100);

    for (size_t i = 0; i < WARMUP_COUNT; ++i) {
        warmup_processor.process(events[i]);
    }

    EventProcessor processor(100);

    const auto start =
        std::chrono::steady_clock::now();

    processor.process_batch(events);

    const auto end =
        std::chrono::steady_clock::now();

    const double elapsed_seconds =
        std::chrono::duration<double>(
            end - start
        ).count();

    const double events_per_second =
        static_cast<double>(EVENT_COUNT)
        / elapsed_seconds;

    const double ns_per_event =
        elapsed_seconds *
        1'000'000'000.0
        /
        static_cast<double>(EVENT_COUNT);

    const auto snapshot =
        processor.order_book().snapshot();

    std::cout
        << std::fixed
        << std::setprecision(2);

    std::cout
        << "\n============================================\n"
        << "       HFT THROUGHPUT BENCHMARK\n"
        << "============================================\n\n";

    std::cout
        << "Events processed:        "
        << EVENT_COUNT
        << '\n';

    std::cout
        << "Elapsed time:            "
        << elapsed_seconds
        << " seconds\n";

    std::cout
        << "Throughput:              "
        << events_per_second
        << " events/sec\n";

    std::cout
        << "Average event time:      "
        << ns_per_event
        << " ns/event\n";

    std::cout
        << "Final mid price:         "
        << snapshot.mid_price
        << '\n';

    std::cout
        << "Final spread:            "
        << snapshot.spread
        << '\n';

    std::cout
        << "Final OBI:               "
        << processor.order_book().imbalance(1)
        << '\n';

    std::cout
        << "\n============================================\n";

    return 0;
}