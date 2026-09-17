# HFT Alpha Research Engine

An event-driven quantitative research system for studying whether
limit-order-book dynamics and order-flow information contain predictive
signals for short-horizon market movements.

The project combines a high-performance C++ limit-order-book replay engine
with Python-based feature engineering, machine learning, and historical
event-replay backtesting.

---

## Key Results

| Research Component | Result |
|---|---:|
| AAPL LOBSTER raw events | 118K+ |
| Events processed through replay/feature pipeline | 107,165 |
| Modeling observations | 21,431 |
| Model ROC-AUC | **0.7121** |
| Model accuracy | **62.64%** |
| Signal transitions | **6,472** |
| Cumulative research backtest return | **44.92%** |
| Maximum drawdown | **−3.99%** |
| C++ replay throughput | **11.08M events/sec** |
| Average replay processing | **~90.3 ns/event** |

> Backtest results are based on a limited historical LOBSTER sample and
> simplified execution assumptions. They are research-sample statistics and
> should not be interpreted as live trading performance.

---

# 1. Research Question

> **Do changes in order-book state and order-flow dynamics provide
> statistically useful information about subsequent short-horizon price
> movements?**

The project investigates whether market microstructure variables such as:

- Order Book Imbalance (OBI)
- Order Flow Imbalance (OFI)
- Microprice
- Bid-ask spread
- Signed order volume
- Event type
- Order direction
- Order quantity

contain predictive information about short-horizon market movements.

The objective is not simply to maximize classification accuracy.

The research pipeline evaluates whether observable order-flow information can
be converted into a measurable directional signal and whether that signal
produces meaningful results under a historical event-replay backtest.

---

# 2. Architecture

The system is organized as an end-to-end quantitative research pipeline:

```text
                    LOBSTER AAPL Data
                           |
                           v
              +-------------------------+
              | C++ Market Event Parser |
              +-------------------------+
                           |
                           v
              +-------------------------+
              | Limit Order Book Replay |
              |        Engine (C++)     |
              +-------------------------+
                           |
              +------------+-------------+
              |                          |
              v                          v
       Order Book State            Event Statistics
              |                          |
              +------------+-------------+
                           |
                           v
              +-------------------------+
              | Microstructure Features |
              | OBI / OFI / Microprice  |
              | Spread / Signed Volume   |
              +-------------------------+
                           |
                           v
              +-------------------------+
              | Python Feature Pipeline |
              +-------------------------+
                           |
                           v
              +-------------------------+
              | Short-Horizon ML Model  |
              |        XGBoost          |
              +-------------------------+
                           |
                           v
              +-------------------------+
              | Signal Generation       |
              +-------------------------+
                           |
                           v
              +-------------------------+
              | Historical Backtest     |
              +-------------------------+
                           |
                           v
              +-------------------------+
              | Performance Analysis    |
              +-------------------------+
