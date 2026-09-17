#include "event_processor.hpp"

#include <algorithm>

namespace hft {


EventProcessor::EventProcessor(
    size_t rolling_window
)
    : rolling_window_(rolling_window) {}


void EventProcessor::process(
    const MarketEvent& event
) {

    if (statistics_.total_events == 0) {

        first_timestamp_ =
            event.timestamp;
    }

    last_timestamp_ =
        event.timestamp;

    order_book_.process_event(event);

    update_statistics(event);

    update_rolling_window(event);

    statistics_.total_events++;
}


void EventProcessor::process_batch(
    const std::vector<MarketEvent>& events
) {

    for (const auto& event : events) {

        process(event);
    }
}


void EventProcessor::update_statistics(
    const MarketEvent& event
) {

    switch (event.type) {

        case EventType::ADD:

            statistics_.add_events++;

            break;

        case EventType::MODIFY:

            statistics_.modify_events++;

            break;

        case EventType::CANCEL:

            statistics_.cancel_events++;

            break;

        case EventType::TRADE:

            statistics_.trade_events++;

            break;
    }


    const uint64_t volume =
        static_cast<uint64_t>(
            event.quantity
        );


    if (event.side == Side::BUY) {

        statistics_.buy_events++;

        statistics_.buy_volume += volume;

        statistics_.signed_volume +=
            static_cast<double>(volume);

    } else {

        statistics_.sell_events++;

        statistics_.sell_volume += volume;

        statistics_.signed_volume -=
            static_cast<double>(volume);
    }


    statistics_.total_volume =
        statistics_.buy_volume
        +
        statistics_.sell_volume;


    if (statistics_.total_volume > 0) {

        statistics_.order_flow_imbalance =
            statistics_.signed_volume
            /
            static_cast<double>(
                statistics_.total_volume
            );
    }


    statistics_.trade_intensity =
        events_per_second();
}


void EventProcessor::update_rolling_window(
    const MarketEvent& event
) {

    recent_events_.push_back(event);


    if (event.side == Side::BUY) {

        rolling_buy_volume_ +=
            event.quantity;

    } else {

        rolling_sell_volume_ +=
            event.quantity;
    }


    while (
        recent_events_.size()
        >
        rolling_window_
    ) {

        const MarketEvent& old =
            recent_events_.front();


        if (old.side == Side::BUY) {

            rolling_buy_volume_ -=
                old.quantity;

        } else {

            rolling_sell_volume_ -=
                old.quantity;
        }


        recent_events_.pop_front();
    }
}


const OrderBook&
EventProcessor::order_book() const {

    return order_book_;
}


const EventStatistics&
EventProcessor::statistics() const {

    return statistics_;
}


double EventProcessor::rolling_signed_volume() const {

    return
        static_cast<double>(
            rolling_buy_volume_
        )
        -
        static_cast<double>(
            rolling_sell_volume_
        );
}


double EventProcessor::rolling_volume_imbalance() const {

    const uint64_t total =
        rolling_buy_volume_
        +
        rolling_sell_volume_;


    if (total == 0) {
        return 0.0;
    }


    return
        rolling_signed_volume()
        /
        static_cast<double>(total);
}


double EventProcessor::events_per_second() const {

    if (
        statistics_.total_events == 0
        ||
        last_timestamp_ <= first_timestamp_
    ) {

        return 0.0;
    }


    // Market timestamps are assumed
    // to be nanoseconds.

    const double elapsed_seconds =
        static_cast<double>(
            last_timestamp_
            -
            first_timestamp_
        )
        /
        1'000'000'000.0;


    if (elapsed_seconds <= 0.0) {
        return 0.0;
    }


    return
        static_cast<double>(
            statistics_.total_events
        )
        /
        elapsed_seconds;
}


void EventProcessor::reset() {

    order_book_.clear();

    statistics_ =
        EventStatistics{};

    recent_events_.clear();

    first_timestamp_ = 0;

    last_timestamp_ = 0;

    rolling_buy_volume_ = 0;

    rolling_sell_volume_ = 0;
}

}