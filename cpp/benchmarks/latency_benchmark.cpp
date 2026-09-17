#include "event_processor.hpp"

#include <algorithm>
#include <chrono>
#include <cstdint>
#include <iomanip>
#include <iostream>
#include <vector>

using namespace hft;

int main() {

    constexpr size_t EVENT_COUNT = 100'000;

    std::vector<MarketEvent> events;
    events.reserve(EVENT_COUNT);

    uint64_t timestamp = 1'000'000'000;

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
                ? 100.00 - (i % 10) * 0.01
                : 100.01 + (i % 10) * 0.01;

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

    std::vector<double> latency_ns;
    latency_ns.reserve(EVENT_COUNT);

    // Warm-up
    for (size_t i = 0; i < 10'000; ++i) {
        processor.process(events[i]);
    }

    processor.reset();

    // Measure each event individually.
    for (size_t i = 0; i < EVENT_COUNT; ++i) {

        const auto start =
            std::chrono::steady_clock::now();

        processor.process(events[i]);

        const auto end =
            std::chrono::steady_clock::now();

        const double ns =
            static_cast<double>(
                std::chrono::duration_cast<
                    std::chrono::nanoseconds
                >(end - start).count()
            );

        latency_ns.push_back(ns);
    }

    std::sort(
        latency_ns.begin(),
        latency_ns.end()
    );

    auto percentile =
        [&](double p) -> double {

            const double index =
                p * static_cast<double>(
                    latency_ns.size() - 1
                );

            const size_t lower =
                static_cast<size_t>(index);

            const size_t upper =
                std::min(
                    lower + 1,
                    latency_ns.size() - 1
                );

            const double fraction =
                index - static_cast<double>(lower);

            return
                latency_ns[lower]
                +
                fraction *
                (
                    latency_ns[upper]
                    -
                    latency_ns[lower]
                );
        };

    double total = 0.0;

    for (double value : latency_ns) {
        total += value;
    }

    const double average =
        total /
        static_cast<double>(
            latency_ns.size()
        );

    const double p50 =
        percentile(0.50);

    const double p95 =
        percentile(0.95);

    const double p99 =
        percentile(0.99);

    const double p999 =
        percentile(0.999);

    const double minimum =
        latency_ns.front();

    const double maximum =
        latency_ns.back();

    std::cout
        << std::fixed
        << std::setprecision(2);

    std::cout
        << "\n============================================\n"
        << "        HFT LATENCY DISTRIBUTION\n"
        << "============================================\n\n";

    std::cout
        << "Events measured:         "
        << EVENT_COUNT
        << '\n';

    std::cout
        << "Minimum latency:         "
        << minimum
        << " ns\n";

    std::cout
        << "Average latency:         "
        << average
        << " ns\n";

    std::cout
        << "P50 latency:             "
        << p50
        << " ns\n";

    std::cout
        << "P95 latency:             "
        << p95
        << " ns\n";

    std::cout
        << "P99 latency:             "
        << p99
        << " ns\n";

    std::cout
        << "P99.9 latency:           "
        << p999
        << " ns\n";

    std::cout
        << "Maximum latency:         "
        << maximum
        << " ns\n";

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