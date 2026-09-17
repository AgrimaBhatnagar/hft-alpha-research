#include "data/lobster_parser.hpp"
#include "fast_order_book.hpp"
#include "ofi_engine.hpp"

#include <chrono>
#include <iomanip>
#include <iostream>

using namespace hft;

int main() {

    const std::string message_file =
        "../data/raw/"
        "LOBSTER_SampleFile_AAPL_2012-06-21_1/"
        "AAPL_2012-06-21_34200000_57600000_message_1.csv";

    std::cout
        << "\n============================================\n"
        << "       AAPL LOBSTER ALPHA RESEARCH\n"
        << "============================================\n\n";

    std::cout << "Loading events...\n";

    LobsterParser parser;

    auto events = parser.parse(message_file);

    std::cout
        << "Events loaded:          "
        << events.size()
        << "\n\n";

    FastOrderBook book;
    OFIEngine ofi;

    const auto start =
        std::chrono::high_resolution_clock::now();

    for (const auto& event : events) {

        book.process_event(event);

        ofi.process(event);
    }

    const auto end =
        std::chrono::high_resolution_clock::now();

    const double elapsed =
        std::chrono::duration<double>(end - start)
            .count();

    const double throughput =
        static_cast<double>(events.size()) /
        elapsed;

    std::cout
        << "REPLAY PERFORMANCE\n"
        << "--------------------------------------------\n";

    std::cout
        << std::fixed
        << std::setprecision(4);

    std::cout
        << "Replay time:            "
        << elapsed
        << " sec\n";

    std::cout
        << "Throughput:             "
        << throughput
        << " events/sec\n";

    std::cout
        << "\nORDER FLOW FEATURES\n"
        << "--------------------------------------------\n";

    std::cout
        << "Events:                 "
        << ofi.event_count()
        << '\n';

    std::cout
        << "Buy volume:             "
        << ofi.buy_volume()
        << '\n';

    std::cout
        << "Sell volume:            "
        << ofi.sell_volume()
        << '\n';

    std::cout
        << "Signed volume:          "
        << ofi.signed_volume()
        << '\n';

    std::cout
        << "Global OFI:             "
        << ofi.global_ofi()
        << '\n';

    std::cout
        << "\nMARKET STATE\n"
        << "--------------------------------------------\n";

    std::cout
        << "Best bid:               "
        << book.best_bid()
        << '\n';

    std::cout
        << "Best ask:               "
        << book.best_ask()
        << '\n';

    std::cout
        << "Mid price:              "
        << book.mid_price()
        << '\n';

    std::cout
        << "Spread:                 "
        << book.spread()
        << '\n';

    std::cout
        << "OBI L1:                 "
        << book.imbalance(1)
        << '\n';

    std::cout
        << "OBI L3:                 "
        << book.imbalance(3)
        << '\n';

    std::cout
        << "\n============================================\n";

    return 0;
}