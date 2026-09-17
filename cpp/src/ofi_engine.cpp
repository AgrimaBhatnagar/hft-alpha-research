#include "ofi_engine.hpp"

namespace hft {

OFIEngine::OFIEngine()
    : buy_volume_(0.0),
      sell_volume_(0.0),
      signed_volume_(0.0),
      event_count_(0) {
}


void OFIEngine::process(
    const MarketEvent& event
) {
    ++event_count_;

    const double quantity =
        static_cast<double>(
            event.quantity
        );

    if (event.side == Side::BUY) {

        buy_volume_ += quantity;
        signed_volume_ += quantity;

    } else {

        sell_volume_ += quantity;
        signed_volume_ -= quantity;
    }
}


double OFIEngine::global_ofi() const {

    const double total =
        buy_volume_ +
        sell_volume_;

    if (total == 0.0) {
        return 0.0;
    }

    return signed_volume_ / total;
}


double OFIEngine::buy_volume() const {
    return buy_volume_;
}


double OFIEngine::sell_volume() const {
    return sell_volume_;
}


double OFIEngine::signed_volume() const {
    return signed_volume_;
}


uint64_t OFIEngine::event_count() const {
    return event_count_;
}


void OFIEngine::reset() {

    buy_volume_ = 0.0;
    sell_volume_ = 0.0;
    signed_volume_ = 0.0;
    event_count_ = 0;
}

}