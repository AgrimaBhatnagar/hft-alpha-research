#include "event_processor.hpp"
#include "microstructure.hpp"

#include <iomanip>
#include <iostream>
#include <vector>

using namespace hft;


int main() {

    EventProcessor processor(5);

    std::vector<MarketEvent> events {

        // -------------------------
        // Initial BUY liquidity
        // -------------------------

        MarketEvent(
            1'000'000'000,
            1001,
            Side::BUY,
            EventType::ADD,
            100.00,
            500
        ),

        MarketEvent(
            1'000'001'000,
            1002,
            Side::BUY,
            EventType::ADD,
            99.99,
            800
        ),

        MarketEvent(
            1'000'002'000,
            1003,
            Side::BUY,
            EventType::ADD,
            99.98,
            1200
        ),

        // -------------------------
        // Initial SELL liquidity
        // -------------------------

        MarketEvent(
            1'000'003'000,
            2001,
            Side::SELL,
            EventType::ADD,
            100.01,
            300
        ),

        MarketEvent(
            1'000'004'000,
            2002,
            Side::SELL,
            EventType::ADD,
            100.02,
            700
        ),

        MarketEvent(
            1'000'005'000,
            2003,
            Side::SELL,
            EventType::ADD,
            100.03,
            1000
        ),

        // -------------------------
        // Order flow
        // -------------------------

        MarketEvent(
            1'000'006'000,
            3001,
            Side::BUY,
            EventType::TRADE,
            100.01,
            150
        ),

        MarketEvent(
            1'000'007'000,
            3002,
            Side::BUY,
            EventType::TRADE,
            100.01,
            100
        ),

        MarketEvent(
            1'000'008'000,
            3003,
            Side::SELL,
            EventType::TRADE,
            100.00,
            80
        ),

        MarketEvent(
            1'000'009'000,
            1002,
            Side::BUY,
            EventType::MODIFY,
            99.99,
            600
        ),

        MarketEvent(
            1'000'010'000,
            2003,
            Side::SELL,
            EventType::CANCEL,
            100.03,
            1000
        )
    };


    processor.process_batch(events);


    const auto& stats =
        processor.statistics();

    const auto& book =
        processor.order_book();


    const auto snapshot =
        book.snapshot();


    std::cout
        << std::fixed
        << std::setprecision(6);


    std::cout
        << "\n============================================\n";

    std::cout
        << "        HFT ALPHA EVENT ENGINE\n";

    std::cout
        << "============================================\n\n";


    std::cout
        << "MARKET STATE\n"
        << "--------------------------------------------\n";

    std::cout
        << "Best Bid:                 "
        << snapshot.best_bid
        << '\n';

    std::cout
        << "Best Ask:                 "
        << snapshot.best_ask
        << '\n';

    std::cout
        << "Mid Price:                "
        << snapshot.mid_price
        << '\n';

    std::cout
        << "Spread:                   "
        << snapshot.spread
        << '\n';

    std::cout
        << "Microprice:               "
        << book.microprice()
        << '\n';

    std::cout
        << "Microprice Deviation:     "
        << book.microprice()
            - snapshot.mid_price
        << '\n';


    std::cout
        << "\nORDER FLOW\n"
        << "--------------------------------------------\n";

    std::cout
        << "Total Events:             "
        << stats.total_events
        << '\n';

    std::cout
        << "ADD Events:               "
        << stats.add_events
        << '\n';

    std::cout
        << "MODIFY Events:            "
        << stats.modify_events
        << '\n';

    std::cout
        << "CANCEL Events:            "
        << stats.cancel_events
        << '\n';

    std::cout
        << "TRADE Events:             "
        << stats.trade_events
        << '\n';

    std::cout
        << "Buy Volume:               "
        << stats.buy_volume
        << '\n';

    std::cout
        << "Sell Volume:              "
        << stats.sell_volume
        << '\n';

    std::cout
        << "Signed Volume:            "
        << stats.signed_volume
        << '\n';

    std::cout
        << "Global OFI:               "
        << stats.order_flow_imbalance
        << '\n';

    std::cout
        << "Rolling OFI:              "
        << processor.rolling_volume_imbalance()
        << '\n';

    std::cout
        << "Events / Second:          "
        << processor.events_per_second()
        << '\n';


    std::cout
        << "\nORDER BOOK\n"
        << "--------------------------------------------\n";

    std::cout
        << "Bid Levels:               "
        << book.bid_levels()
        << '\n';

    std::cout
        << "Ask Levels:               "
        << book.ask_levels()
        << '\n';

    std::cout
        << "Total Bid Volume:         "
        << book.total_bid_volume()
        << '\n';

    std::cout
        << "Total Ask Volume:         "
        << book.total_ask_volume()
        << '\n';

    std::cout
        << "OBI Level 1:              "
        << book.imbalance(1)
        << '\n';

    std::cout
        << "OBI Level 3:              "
        << book.imbalance(3)
        << '\n';


    std::cout
        << "\n============================================\n";


    return 0;
}