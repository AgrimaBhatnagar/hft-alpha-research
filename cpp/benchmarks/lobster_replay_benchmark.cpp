#include "data/lobster_parser.hpp"
#include "fast_order_book.hpp"

#include <chrono>
#include <iomanip>
#include <iostream>
#include <string>

using namespace hft;


int main() {

    const std::string filename =
    "../data/raw/"
    "LOBSTER_SampleFile_AAPL_2012-06-21_1/"
    "AAPL_2012-06-21_34200000_57600000_message_1.csv";


    std::cout
        << "\n============================================\n"
        << "       REAL AAPL LOBSTER REPLAY\n"
        << "============================================\n\n";


    std::cout
        << "Loading real market events...\n";


    LobsterParser parser;


    const auto load_start =
        std::chrono::steady_clock::now();


    auto events =
        parser.parse(filename);


    const auto load_end =
        std::chrono::steady_clock::now();


    const double load_seconds =
        std::chrono::duration<double>(
            load_end - load_start
        ).count();


    std::cout
        << "Events loaded:          "
        << events.size()
        << '\n';


    std::cout
        << "CSV load time:          "
        << std::fixed
        << std::setprecision(4)
        << load_seconds
        << " sec\n\n";


    if (events.empty()) {

        std::cerr
            << "ERROR: No events loaded.\n";

        return 1;
    }


    FastOrderBook book;


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


    const double event_count =
        static_cast<double>(
            events.size()
        );


    const double throughput =
        event_count / seconds;


    const double ns_per_event =
        seconds *
        1'000'000'000.0 /
        event_count;


    std::cout
        << "REPLAY PERFORMANCE\n"
        << "--------------------------------------------\n";


    std::cout
        << "Replay time:            "
        << seconds
        << " sec\n";


    std::cout
        << "Throughput:             "
        << throughput
        << " events/sec\n";


    std::cout
        << "Average processing:     "
        << ns_per_event
        << " ns/event\n\n";


    std::cout
        << "FINAL RECONSTRUCTED BOOK\n"
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