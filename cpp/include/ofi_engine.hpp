#pragma once

#include "market_event.hpp"

#include <cstdint>

namespace hft {

class OFIEngine {
public:

    OFIEngine();

    void process(
        const MarketEvent& event
    );

    double global_ofi() const;

    double buy_volume() const;

    double sell_volume() const;

    double signed_volume() const;

    uint64_t event_count() const;

    void reset();

private:

    double buy_volume_;
    double sell_volume_;
    double signed_volume_;

    uint64_t event_count_;
};

}