from dataclasses import dataclass
from typing import List
import csv
import os
import time


@dataclass
class PriceLevel:
    price: float
    quantity: int


class ResearchEngine:

    def __init__(self):

        self.base_dir = os.path.abspath(
            os.path.join(
                os.path.dirname(__file__),
                "..",
                ".."
            )
        )

        self.prediction_file = os.path.join(
            self.base_dir,
            "data",
            "processed",
            "aapl_alpha_predictions.csv"
        )

        self.backtest_file = os.path.join(
            self.base_dir,
            "data",
            "processed",
            "backtest_results.csv"
        )

        self.data = []
        self.backtest_data = []
        self.backtest_by_timestamp = {}

        self.data_index = 0

        self.reset()
        self.load_real_data()

    # ============================================================
    # RESET
    # ============================================================

    def reset(self):

        self.bids: List[PriceLevel] = []
        self.asks: List[PriceLevel] = []

        self.total_events = 0

        self.buy_volume = 0.0
        self.sell_volume = 0.0
        self.signed_volume = 0.0

        self.event_rate = 0.0
        self.latency_ns = 0.0

        self.global_ofi = 0.0

        self.alpha_probability = 0.5
        self.prediction = 0
        self.signal = 0

        self.strategy_return = 0.0
        self.equity = 1.0

        self.dataset = "AAPL LOBSTER"

        self.price_history = []
        self.obi_history = []
        self.alpha_probability_history = []
        self.equity_history = []

    # ============================================================
    # LOAD DATA
    # ============================================================

    def load_real_data(self):

        self.data = []
        self.backtest_data = []
        self.backtest_by_timestamp = {}

        if not os.path.exists(self.prediction_file):
            raise FileNotFoundError(
                f"Missing prediction file: {self.prediction_file}"
            )

        with open(
            self.prediction_file,
            "r",
            newline="",
            encoding="utf-8"
        ) as f:

            reader = csv.DictReader(f)

            for row in reader:
                self.data.append(row)

        if os.path.exists(self.backtest_file):

            with open(
                self.backtest_file,
                "r",
                newline="",
                encoding="utf-8"
            ) as f:

                reader = csv.DictReader(f)

                for row in reader:

                    self.backtest_data.append(row)

                    timestamp = row.get(
                        "timestamp",
                        ""
                    )

                    if timestamp:
                        self.backtest_by_timestamp[
                            timestamp
                        ] = row

        if not self.data:
            raise ValueError(
                "AAPL prediction dataset is empty."
            )

        self.data_index = 0

        self._load_row(
            self.data_index
        )

    # ============================================================
    # SAFE CONVERSION
    # ============================================================

    @staticmethod
    def _float(
        row,
        key,
        default=0.0
    ):

        try:

            value = row.get(
                key,
                ""
            )

            if value in ("", None):
                return default

            return float(value)

        except (
            ValueError,
            TypeError
        ):

            return default

    @staticmethod
    def _int(
        row,
        key,
        default=0
    ):

        try:

            value = row.get(
                key,
                ""
            )

            if value in ("", None):
                return default

            return int(
                float(value)
            )

        except (
            ValueError,
            TypeError
        ):

            return default

    # ============================================================
    # DISPLAY ORDER BOOK
    # ============================================================

    def _build_display_book(
        self,
        raw_bid,
        raw_ask,
        quantity,
        obi
    ):

        self.bids = []
        self.asks = []

        if raw_bid <= 0:
            return

        # Prevent crossed/invalid display book.
        if raw_ask <= raw_bid:

            raw_ask = round(
                raw_bid + 0.01,
                2
            )

        quantity = max(
            1,
            int(quantity)
        )

        bid_multiplier = max(
            0.25,
            1.0 + obi
        )

        ask_multiplier = max(
            0.25,
            1.0 - obi
        )

        for level in range(8):

            bid_price = round(
                raw_bid -
                level * 0.01,
                2
            )

            ask_price = round(
                raw_ask +
                level * 0.01,
                2
            )

            depth_factor = (
                1.0 +
                level * 0.08
            )

            bid_qty = max(
                1,
                int(
                    quantity *
                    bid_multiplier *
                    depth_factor
                )
            )

            ask_qty = max(
                1,
                int(
                    quantity *
                    ask_multiplier *
                    depth_factor
                )
            )

            self.bids.append(
                PriceLevel(
                    bid_price,
                    bid_qty
                )
            )

            self.asks.append(
                PriceLevel(
                    ask_price,
                    ask_qty
                )
            )

    # ============================================================
    # LOAD EVENT
    # ============================================================

    def _load_row(
        self,
        index
    ):

        if not self.data:
            return

        index = max(
            0,
            min(
                index,
                len(self.data) - 1
            )
        )

        row = self.data[index]

        raw_bid = self._float(
            row,
            "best_bid"
        )

        raw_ask = self._float(
            row,
            "best_ask"
        )

        quantity = max(
            1,
            self._int(
                row,
                "quantity",
                100
            )
        )

        obi = self._float(
            row,
            "obi_l1"
        )

        self._build_display_book(
            raw_bid,
            raw_ask,
            quantity,
            obi
        )

        # --------------------------------------------------------
        # FLOW
        # --------------------------------------------------------

        self.buy_volume = self._float(
            row,
            "buy_volume"
        )

        self.sell_volume = self._float(
            row,
            "sell_volume"
        )

        self.signed_volume = self._float(
            row,
            "signed_volume"
        )

        self.global_ofi = self._float(
            row,
            "global_ofi"
        )

        # --------------------------------------------------------
        # MODEL
        # --------------------------------------------------------

        self.alpha_probability = self._float(
            row,
            "alpha_probability",
            0.5
        )

        self.prediction = self._int(
            row,
            "prediction"
        )

        # --------------------------------------------------------
        # BACKTEST
        # --------------------------------------------------------

        timestamp = row.get(
            "timestamp",
            ""
        )

        backtest_row = (
            self.backtest_by_timestamp.get(
                timestamp
            )
        )

        if backtest_row:

            self.signal = self._int(
                backtest_row,
                "signal",
                self.prediction
            )

            self.strategy_return = self._float(
                backtest_row,
                "strategy_return"
            )

            equity = self._float(
                backtest_row,
                "equity",
                self.equity
            )

            if equity > 0:
                self.equity = equity

        else:

            self.signal = self.prediction
            self.strategy_return = 0.0

        # --------------------------------------------------------
        # EVENT COUNT
        # --------------------------------------------------------

        self.total_events = index + 1

        # --------------------------------------------------------
        # REAL EVENT RATE
        #
        # LOBSTER timestamps here are nanoseconds.
        # --------------------------------------------------------

        if index > 0:

            previous = self.data[
                index - 1
            ]

            previous_ts = self._float(
                previous,
                "timestamp"
            )

            current_ts = self._float(
                row,
                "timestamp"
            )

            delta = (
                current_ts -
                previous_ts
            )

            if delta > 0:

                self.event_rate = min(
                    1_000_000_000.0 / delta,
                    10_000_000.0
                )

        if self.event_rate <= 0:
            self.event_rate = 1_000_000.0

        # --------------------------------------------------------
        # Python API processing latency
        # --------------------------------------------------------

        start = time.perf_counter()

        _ = (
            self.buy_volume +
            self.sell_volume +
            self.global_ofi
        )

        elapsed = (
            time.perf_counter() -
            start
        )

        self.latency_ns = max(
            elapsed * 1_000_000_000,
            1.0
        )

        self._record_state()

    # ============================================================
    # REPLAY
    # ============================================================

    def replay_next(self):

        if not self.data:
            return self.snapshot()

        self.data_index += 1

        if self.data_index >= len(
            self.data
        ):

            self.data_index = 0

            self.price_history = []
            self.obi_history = []
            self.alpha_probability_history = []
            self.equity_history = []

        start = time.perf_counter()

        self._load_row(
            self.data_index
        )

        elapsed = (
            time.perf_counter() -
            start
        )

        self.latency_ns = max(
            elapsed * 1_000_000_000,
            1.0
        )

        return self.snapshot()

    # Backward compatibility
    def simulate_market_update(self):

        return self.replay_next()

    # ============================================================
    # HISTORY
    # ============================================================

    def _record_state(self):

        self.price_history.append(
            self.mid_price()
        )

        self.obi_history.append(
            self.obi()
        )

        self.alpha_probability_history.append(
            self.alpha_probability
        )

        self.equity_history.append(
            self.equity
        )

        self.price_history = (
            self.price_history[-120:]
        )

        self.obi_history = (
            self.obi_history[-120:]
        )

        self.alpha_probability_history = (
            self.alpha_probability_history[-120:]
        )

        self.equity_history = (
            self.equity_history[-120:]
        )

    # ============================================================
    # MARKET FUNCTIONS
    # ============================================================

    def best_bid(self):

        if not self.bids:
            return 0.0

        return self.bids[0].price

    def best_ask(self):

        if not self.asks:
            return 0.0

        return self.asks[0].price

    def mid_price(self):

        bid = self.best_bid()
        ask = self.best_ask()

        if bid <= 0 or ask <= 0:
            return 0.0

        return (
            bid + ask
        ) / 2.0

    def spread(self):

        bid = self.best_bid()
        ask = self.best_ask()

        if bid <= 0 or ask <= 0:
            return 0.0

        return max(
            0.0,
            ask - bid
        )

    def relative_spread(self):

        mid = self.mid_price()

        if mid <= 0:
            return 0.0

        return (
            self.spread() /
            mid
        )

    def obi(self):

        if not self.bids or not self.asks:
            return 0.0

        bid = self.bids[0].quantity
        ask = self.asks[0].quantity

        total = bid + ask

        if total <= 0:
            return 0.0

        return (
            bid - ask
        ) / total

    def obi_l3(self):

        bid = sum(
            x.quantity
            for x in self.bids[:3]
        )

        ask = sum(
            x.quantity
            for x in self.asks[:3]
        )

        total = bid + ask

        if total <= 0:
            return 0.0

        return (
            bid - ask
        ) / total

    def microprice(self):

        if not self.bids or not self.asks:
            return 0.0

        bid = self.best_bid()
        ask = self.best_ask()

        bid_size = self.bids[0].quantity
        ask_size = self.asks[0].quantity

        total = bid_size + ask_size

        if total <= 0:
            return (
                bid + ask
            ) / 2.0

        return (
            ask * bid_size +
            bid * ask_size
        ) / total

    def microprice_deviation(self):

        return (
            self.microprice() -
            self.mid_price()
        )

    # ============================================================
    # SNAPSHOT
    # ============================================================

    def snapshot(self):

        return {

            "dataset":
                self.dataset,

            "events":
                self.total_events,

            "events_available":
                len(self.data),

            "bids": [
                {
                    "price": x.price,
                    "quantity": x.quantity
                }
                for x in self.bids
            ],

            "asks": [
                {
                    "price": x.price,
                    "quantity": x.quantity
                }
                for x in self.asks
            ],

            "best_bid":
                self.best_bid(),

            "best_ask":
                self.best_ask(),

            "mid_price":
                self.mid_price(),

            "spread":
                self.spread(),

            "relative_spread":
                self.relative_spread(),

            "microprice":
                self.microprice(),

            "microprice_deviation":
                self.microprice_deviation(),

            "obi":
                self.obi(),

            "obi_l3":
                self.obi_l3(),

            "global_ofi":
                self.global_ofi,

            "bid_depth":
                sum(
                    x.quantity
                    for x in self.bids
                ),

            "ask_depth":
                sum(
                    x.quantity
                    for x in self.asks
                ),

            "buy_volume":
                self.buy_volume,

            "sell_volume":
                self.sell_volume,

            "signed_volume":
                self.signed_volume,

            "alpha_probability":
                self.alpha_probability,

            "prediction":
                self.prediction,

            "signal":
                self.signal,

            "strategy_return":
                self.strategy_return,

            "equity":
                self.equity,

            "prediction_rows":
                len(self.data),

            "backtest_rows":
                len(self.backtest_data),

            "event_rate":
                self.event_rate,

            "latency_ns":
                self.latency_ns,

            "price_history":
                self.price_history,

            "obi_history":
                self.obi_history,

            "alpha_probability_history":
                self.alpha_probability_history,

            "equity_history":
                self.equity_history
        }