#include "data/lobster_parser.hpp"
#include "order_aware_book.hpp"

#include <cmath>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>

using namespace hft;


struct ReferenceBook {
    double ask_price;
    uint64_t ask_volume;

    double bid_price;
    uint64_t bid_volume;
};


bool read_reference_row(
    const std::string& line,
    ReferenceBook& ref
) {
    std::stringstream ss(line);

    std::string field;

    try {

        std::getline(ss, field, ',');
        ref.ask_price =
            std::stod(field) / 10000.0;

        std::getline(ss, field, ',');
        ref.ask_volume =
            std::stoull(field);

        std::getline(ss, field, ',');
        ref.bid_price =
            std::stod(field) / 10000.0;

        std::getline(ss, field, ',');
        ref.bid_volume =
            std::stoull(field);

    } catch (...) {
        return false;
    }

    return true;
}


int main() {

    const std::string message_file =
        "../data/raw/"
        "LOBSTER_SampleFile_AAPL_2012-06-21_1/"
        "AAPL_2012-06-21_34200000_57600000_message_1.csv";

    const std::string book_file =
        "../data/raw/"
        "LOBSTER_SampleFile_AAPL_2012-06-21_1/"
        "AAPL_2012-06-21_34200000_57600000_orderbook_1.csv";


    LobsterParser parser;

    auto events =
        parser.parse(message_file);


    std::ifstream book_stream(book_file);

    if (!book_stream.is_open()) {

        std::cerr
            << "Could not open orderbook file.\n";

        return 1;
    }


    std::string line;

    ReferenceBook initial;

    if (!std::getline(book_stream, line) ||
        !read_reference_row(line, initial)) {

        std::cerr
            << "Could not read initial book.\n";

        return 1;
    }


    /*
        The first orderbook snapshot already
        reflects the first message event.

        Initialize our book from that snapshot
        and start replaying from event #2.
    */

    OrderAwareBook book;

    book.initialize_level(
        Side::SELL,
        initial.ask_price,
        initial.ask_volume
    );

    book.initialize_level(
        Side::BUY,
        initial.bid_price,
        initial.bid_volume
    );


    std::size_t rows = 1;

    std::size_t bid_price_matches = 1;
    std::size_t ask_price_matches = 1;

    std::size_t bid_volume_matches = 1;
    std::size_t ask_volume_matches = 1;

    std::size_t exact_matches = 1;

    std::size_t first_mismatch =
        0;


    const double EPS = 0.00001;


    /*
        Event 0 is already represented by
        the first reference snapshot.

        Start at event index 1.
    */

    for (
        std::size_t i = 1;
        i < events.size();
        ++i
    ) {

        if (!std::getline(
                book_stream,
                line
            )) {
            break;
        }


        ReferenceBook ref;

        if (!read_reference_row(
                line,
                ref
            )) {
            continue;
        }


        book.process_event(
            events[i]
        );


        const bool bid_price_ok =
            std::abs(
                book.best_bid() -
                ref.bid_price
            ) < EPS;


        const bool ask_price_ok =
            std::abs(
                book.best_ask() -
                ref.ask_price
            ) < EPS;


        const bool bid_volume_ok =
            book.best_bid_volume() ==
            ref.bid_volume;


        const bool ask_volume_ok =
            book.best_ask_volume() ==
            ref.ask_volume;


        if (bid_price_ok)
            ++bid_price_matches;

        if (ask_price_ok)
            ++ask_price_matches;

        if (bid_volume_ok)
            ++bid_volume_matches;

        if (ask_volume_ok)
            ++ask_volume_matches;


        const bool exact =
            bid_price_ok &&
            ask_price_ok &&
            bid_volume_ok &&
            ask_volume_ok;


        if (exact) {

            ++exact_matches;

        } else if (
            first_mismatch == 0
        ) {

            first_mismatch =
                i + 1;

            std::cout
                << "\nFIRST MISMATCH\n"
                << "--------------------------------------------\n";

            std::cout
                << "Event:       "
                << i + 1
                << '\n';

            std::cout
                << "Event type:  "
                << static_cast<int>(
                    events[i].type
                )
                << '\n';

            std::cout
                << "Order ID:    "
                << events[i].order_id
                << '\n';

            std::cout
                << "Price:       "
                << events[i].price
                << '\n';

            std::cout
                << "Quantity:    "
                << events[i].quantity
                << '\n';

            std::cout
                << "Direction:   "
                << (
                    events[i].side == Side::BUY
                        ? "BUY"
                        : "SELL"
                )
                << "\n\n";

            std::cout
                << "OUR BOOK\n";

            std::cout
                << "Bid: "
                << book.best_bid()
                << " x "
                << book.best_bid_volume()
                << '\n';

            std::cout
                << "Ask: "
                << book.best_ask()
                << " x "
                << book.best_ask_volume()
                << "\n\n";

            std::cout
                << "LOBSTER\n";

            std::cout
                << "Bid: "
                << ref.bid_price
                << " x "
                << ref.bid_volume
                << '\n';

            std::cout
                << "Ask: "
                << ref.ask_price
                << " x "
                << ref.ask_volume
                << "\n";
        }


        ++rows;
    }


    const double total =
        static_cast<double>(rows);


    std::cout
        << "\n============================================\n"
        << "       LOBSTER L1 VALIDATION\n"
        << "============================================\n\n";


    std::cout
        << "Rows checked:          "
        << rows
        << '\n';


    std::cout
        << std::fixed
        << std::setprecision(2);


    std::cout
        << "Bid price accuracy:    "
        << 100.0 *
            bid_price_matches /
            total
        << "%\n";


    std::cout
        << "Ask price accuracy:    "
        << 100.0 *
            ask_price_matches /
            total
        << "%\n";


    std::cout
        << "Bid volume accuracy:   "
        << 100.0 *
            bid_volume_matches /
            total
        << "%\n";


    std::cout
        << "Ask volume accuracy:   "
        << 100.0 *
            ask_volume_matches /
            total
        << "%\n";


    std::cout
        << "Exact L1 accuracy:     "
        << 100.0 *
            exact_matches /
            total
        << "%\n";


    std::cout
        << "\n============================================\n";


    return 0;
}