#include "microstructure.hpp"

namespace hft {

double MicrostructureEngine::spread(
    const OrderBook& book
) {

    return book.snapshot().spread;
}


double MicrostructureEngine::relative_spread(
    const OrderBook& book
) {

    const auto snapshot =
        book.snapshot();

    if (snapshot.mid_price == 0.0) {
        return 0.0;
    }

    return
        snapshot.spread
        /
        snapshot.mid_price;
}


double MicrostructureEngine::mid_price(
    const OrderBook& book
) {

    return book.snapshot().mid_price;
}


double MicrostructureEngine::microprice(
    const OrderBook& book
) {

    return book.microprice();
}


double MicrostructureEngine::imbalance(
    const OrderBook& book,
    size_t levels
) {

    return book.imbalance(levels);
}


double MicrostructureEngine::microprice_deviation(
    const OrderBook& book
) {

    return
        book.microprice()
        -
        book.snapshot().mid_price;
}

}