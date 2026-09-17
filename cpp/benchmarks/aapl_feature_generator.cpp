#include "data/lobster_parser.hpp"
#include "fast_order_book.hpp"
#include "ofi_engine.hpp"

#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <string>

using namespace hft;

int main() {

    const std::string input_file =
        "../data/raw/"
        "LOBSTER_SampleFile_AAPL_2012-06-21_1/"
        "AAPL_2012-06-21_34200000_57600000_message_1.csv";

    const std::string output_dir =
        "../data/processed";

    const std::string output_file =
        output_dir + "/aapl_features.csv";


    std::filesystem::create_directories(
        output_dir
    );


    std::cout
        << "\n============================================\n"
        << "       AAPL FEATURE GENERATOR\n"
        << "============================================\n\n";


    std::cout
        << "Loading events...\n";


    LobsterParser parser;

    auto events =
        parser.parse(input_file);


    std::cout
        << "Events loaded:          "
        << events.size()
        << "\n";


    if (events.empty()) {

        std::cerr
            << "No events loaded.\n";

        return 1;
    }


    FastOrderBook book;
    OFIEngine ofi;


    std::ofstream output(
        output_file
    );


    if (!output.is_open()) {

        std::cerr
            << "Could not create output file:\n"
            << output_file
            << '\n';

        return 1;
    }


    output
        << "timestamp,"
        << "event_type,"
        << "side,"
        << "price,"
        << "quantity,"
        << "best_bid,"
        << "best_ask,"
        << "mid_price,"
        << "spread,"
        << "microprice,"
        << "obi_l1,"
        << "obi_l3,"
        << "buy_volume,"
        << "sell_volume,"
        << "signed_volume,"
        << "global_ofi\n";


    std::size_t written = 0;


    for (const auto& event : events) {

        book.process_event(event);

        ofi.process(event);


        const double bid =
            book.best_bid();

        const double ask =
            book.best_ask();

        const double mid =
            book.mid_price();

        const double spread =
            book.spread();

        const double microprice =
            book.microprice();

        const double obi_l1 =
            book.imbalance(1);

        const double obi_l3 =
            book.imbalance(3);


        output
            << std::setprecision(12)

            << event.timestamp
            << ","

            << static_cast<int>(
                event.type
            )
            << ","

            << static_cast<int>(
                event.side
            )
            << ","

            << event.price
            << ","

            << event.quantity
            << ","

            << bid
            << ","

            << ask
            << ","

            << mid
            << ","

            << spread
            << ","

            << microprice
            << ","

            << obi_l1
            << ","

            << obi_l3
            << ","

            << ofi.buy_volume()
            << ","

            << ofi.sell_volume()
            << ","

            << ofi.signed_volume()
            << ","

            << ofi.global_ofi()

            << '\n';


        ++written;
    }


    output.close();


    std::cout
        << "\nFEATURE GENERATION COMPLETE\n"
        << "--------------------------------------------\n";

    std::cout
        << "Rows written:           "
        << written
        << '\n';

    std::cout
        << "Output:\n"
        << output_file
        << '\n';

    std::cout
        << "\n============================================\n";


    return 0;
}