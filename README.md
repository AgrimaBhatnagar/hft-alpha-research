# HFT Alpha Research Engine

### Hawkes Process–Informed Order Flow Detection

An event-driven quantitative research system for studying whether limit-order-book dynamics and order-flow information contain predictive signals for short-horizon market movements.

The project combines a high-performance C++ limit-order-book replay engine with Python-based feature engineering, Hawkes-process analysis, machine learning, signal generation, and historical event-replay backtesting.

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

> **Research note:** Backtest results are based on a limited historical LOBSTER sample and simplified execution assumptions. They are research-sample statistics and should not be interpreted as live trading performance.

---

# 1. Research Question

> **Do changes in order-book state and order-flow dynamics provide statistically useful information about subsequent short-horizon price movements?**

The project investigates whether market microstructure variables such as:

- Order Book Imbalance (OBI)
- Order Flow Imbalance (OFI)
- Microprice
- Bid-ask spread
- Signed order volume
- Event type
- Order direction
- Order quantity
- Event intensity and excitation dynamics

contain predictive information about short-horizon market movements.

The objective is not simply to maximize classification accuracy.

The research pipeline evaluates whether observable order-flow information can be converted into a measurable directional signal and whether that signal produces meaningful results under a historical event-replay backtest.

---

# 2. Architecture

The system is organized as an end-to-end quantitative research pipeline:

```text
                         LOBSTER AAPL Data
                                |
                                v
                  +-------------------------+
                  | C++ Market Event Parser |
                  +------------+------------+
                               |
                               v
                  +-------------------------+
                  | Limit Order Book Replay |
                  |       Engine (C++)      |
                  +------------+------------+
                               |
                    +----------+----------+
                    |                     |
                    v                     v
             Order Book State      Event Statistics
                    |                     |
                    +----------+----------+
                               |
                               v
                  +-------------------------+
                  | Microstructure Features |
                  |                         |
                  | OBI / OFI / Microprice  |
                  | Spread / Signed Volume  |
                  +------------+------------+
                               |
                               v
                  +-------------------------+
                  | Hawkes Process Analysis |
                  |                         |
                  | Self-Excitation         |
                  | Cross-Excitation        |
                  | Event Intensity         |
                  +------------+------------+
                               |
                               v
                  +-------------------------+
                  | Python Feature Pipeline |
                  +------------+------------+
                               |
                               v
                  +-------------------------+
                  | Short-Horizon ML Model  |
                  |        XGBoost          |
                  +------------+------------+
                               |
                               v
                  +-------------------------+
                  | Signal Generation       |
                  +------------+------------+
                               |
                               v
                  +-------------------------+
                  | Historical Backtest     |
                  |    Event Replay         |
                  +------------+------------+
                               |
                               v
                  +-------------------------+
                  | Performance Analysis    |
                  +-------------------------+
```

---

# 3. Research Methodology

## 3.1 Market Event Replay

The C++ engine processes event-level AAPL limit-order-book messages and reconstructs the evolving order-book state.

The replay layer maintains and updates:

- Bid prices
- Ask prices
- Bid quantities
- Ask quantities
- Order-level state
- Event timestamps
- Event types
- Order direction
- Order quantities

The event-driven implementation is designed to process market events sequentially while minimizing unnecessary computation during historical replay.

---

## 3.2 Limit Order Book Features

The replay and feature-generation pipeline derives market microstructure variables from the evolving order book.

### Order Book Imbalance

Measures the relative difference between bid-side and ask-side liquidity.

```text
OBI = (Bid Volume - Ask Volume)
      / (Bid Volume + Ask Volume)
```

### Order Flow Imbalance

Captures changes in buying and selling pressure resulting from order-book events.

### Microprice

Combines best bid and ask prices with queue sizes to estimate a liquidity-weighted reference price.

### Additional Features

The feature pipeline also considers:

- Bid-ask spread
- Signed order volume
- Order quantity
- Event type
- Order direction
- Short-horizon price changes
- Event-flow statistics

---

# 4. Hawkes Process Analysis

High-frequency order-book events exhibit temporal clustering, where the occurrence of one event can affect the probability of subsequent events.

The project investigates this behavior using Hawkes-process concepts.

The multivariate Hawkes framework models event intensity as a combination of:

- Baseline intensity
- Self-excitation
- Cross-excitation
- Event-type interactions

Conceptually:

```text
Past Events
     |
     v
+----------------------+
| Excitation Dynamics  |
|                      |
| Self-excitation      |
| Cross-excitation     |
+----------+-----------+
           |
           v
   Current Event
   Intensity
```

The resulting event-dynamics information is evaluated alongside conventional market microstructure variables to investigate whether temporal order-flow structure contains additional predictive information.

---

# 5. Machine Learning Model

The research pipeline uses an **XGBoost classifier** to model short-horizon directional price movement.

The model is trained using observations derived from the event-replay and feature-engineering pipeline.

### Modeling Dataset

```text
Raw market events
        |
        v
Order-book reconstruction
        |
        v
Feature generation
        |
        v
Label generation
        |
        v
21,431 modeling observations
        |
        v
XGBoost classifier
```

Model performance is evaluated using:

- ROC-AUC
- Accuracy
- Directional predictions
- Signal transitions

The goal is to evaluate predictive information in market microstructure rather than optimize for a single performance metric.

---

# 6. Signal Generation

Model predictions are converted into directional research signals according to the experimental configuration.

The signal-generation layer tracks changes in predicted market direction and produces a sequence of event-driven signals for the historical backtest.

A total of:

**6,472 signal transitions**

were observed in the current research configuration.

---

# 7. Backtesting Methodology

The generated signals are evaluated sequentially against the historical event stream using an event-replay backtesting framework.

The backtest preserves the temporal ordering of market events rather than randomly shuffling observations.

The performance layer tracks:

- Position changes
- Signal transitions
- Returns
- Cumulative performance
- Drawdown
- Historical strategy behavior

### Important Execution Assumptions

The current research backtest uses simplified execution assumptions.

It does not fully model:

- Transaction costs
- Market impact
- Queue position
- Partial fills
- Order-book latency
- Exchange fees
- Slippage under live conditions
- Adverse selection

Therefore, the reported return should be interpreted as a **historical research-sample result**, not as expected live-trading performance.

---

# 8. Performance Evaluation

## Predictive Performance

| Metric | Result |
|---|---:|
| Modeling observations | **21,431** |
| ROC-AUC | **0.7121** |
| Accuracy | **62.64%** |

## Research Backtest

| Metric | Result |
|---|---:|
| Signal transitions | **6,472** |
| Cumulative return | **44.92%** |
| Maximum drawdown | **−3.99%** |

## C++ Replay Performance

| Metric | Result |
|---|---:|
| AAPL raw events | **118K+** |
| Events processed | **107,165** |
| Replay throughput | **11.08M events/sec** |
| Average processing time | **~90.3 ns/event** |

---

# 9. Reproducibility

The research workflow is separated into the following stages:

```text
1. Market-data ingestion
2. Event parsing
3. Limit-order-book reconstruction
4. Microstructure feature generation
5. Label creation
6. Hawkes-process analysis
7. Model training
8. Signal generation
9. Historical backtesting
10. Performance analysis
```

This separation allows individual stages of the research pipeline to be tested and benchmarked independently.

---

# 10. Repository Structure

```text
hft-alpha-research/
│
├── api/
│   └── app.py
│
├── backend/
│   ├── app/
│   │   ├── __init__.py
│   │   ├── engine.py
│   │   └── main.py
│   │
│   └── requirements.txt
│
├── cpp/
│   ├── include/
│   │   ├── event_processor.hpp
│   │   ├── fast_order_book.hpp
│   │   ├── market_event.hpp
│   │   ├── microstructure.hpp
│   │   ├── ofi_engine.hpp
│   │   ├── order_aware_book.hpp
│   │   ├── order_book.hpp
│   │   └── order_registry.hpp
│   │
│   ├── src/
│   ├── benchmarks/
│   └── tests/
│
├── frontend/
│   └── index.html
│
├── python/
│   ├── backtest.py
│   ├── create_labels.py
│   └── train_alpha.py
│
├── .gitignore
└── README.md
```

---

# 11. Technology Stack

### Systems & Performance

- C++17
- CMake
- Event-driven processing
- Limit-order-book reconstruction

### Quantitative Research

- Python
- Hawkes Processes
- Market Microstructure
- Event-driven Backtesting
- Quantitative Signal Research

### Machine Learning

- XGBoost
- scikit-learn
- NumPy
- Pandas

### Data

- LOBSTER
- AAPL limit-order-book data

---

# 12. C++ Build

Navigate to the C++ directory:

```bash
cd cpp
```

Create a build directory:

```bash
mkdir build
cd build
```

Configure the project:

```bash
cmake ..
```

Build in Release mode:

```bash
cmake --build . --config Release
```

---

# 13. Python Environment

Create and activate a virtual environment:

```bash
python -m venv .venv
```

### Windows

```bash
.venv\Scripts\activate
```

### Install dependencies

```bash
pip install -r backend/requirements.txt
```

Additional dependencies may be required for individual research experiments.

---

# 14. Testing and Benchmarks

The C++ implementation contains unit tests under:

```text
cpp/tests/
```

The repository also contains dedicated benchmark programs under:

```text
cpp/benchmarks/
```

These benchmarks evaluate components including:

- Event processing
- Order-book replay
- Feature generation
- Throughput
- Latencies
- Replay performance

The measured replay throughput in the current benchmark configuration is:

**11.08M events/sec**

with approximately:

**90.3 ns/event**

average processing time.

---

# 15. Data Availability

The project uses event-level market data from **LOBSTER**.

Raw LOBSTER data is **not included in this repository** because of dataset licensing and redistribution restrictions.

Researchers wishing to reproduce the experiments should obtain the appropriate dataset directly from the data provider and place it in the expected local data directory.

The repository intentionally excludes raw market data from version control.

---

# 16. Limitations

This project is a quantitative research prototype rather than a production trading system.

Important limitations include:

- Limited historical sample size
- Single-asset experimental evaluation
- Simplified execution assumptions
- No full market-impact model
- No queue-position simulation
- No complete partial-fill simulation
- No live exchange latency model
- Potential sensitivity to model parameters
- Potential overfitting to the research sample
- Limited evidence across different market regimes

Further out-of-sample and walk-forward validation would be required to evaluate robustness.

---

# 17. Future Work

Potential extensions include:

- Multi-day validation
- Multi-asset validation
- Walk-forward testing
- Transaction-cost-aware backtesting
- Queue-position modeling
- Partial-fill simulation
- Latency-aware execution modeling
- Online Hawkes-process estimation
- Nonlinear Hawkes processes
- Regime-dependent excitation models
- Cross-asset order-flow analysis
- GPU-accelerated feature computation
- Live market-data integration

---

# 18. Research Interpretation

The results should be interpreted in the context of the experimental design.

The **0.7121 ROC-AUC** indicates measurable predictive discrimination within the evaluated research sample.

The **62.64% accuracy** reflects classification performance under the selected labeling and validation configuration.

The **44.92% cumulative backtest return** and **−3.99% maximum drawdown** describe the behavior of the historical research backtest under its simplified execution assumptions.

The **11.08M events/sec C++ throughput** measures the performance of the replay implementation under the benchmark configuration and should not be interpreted as an end-to-end trading-system latency measurement.

---

# 19. Disclaimer

This repository is intended for educational and quantitative research purposes only.

Historical backtest results do not guarantee future performance and should not be interpreted as investment advice or as evidence of deployable trading profitability.

The project does not constitute a recommendation to buy, sell, or trade any financial instrument.

---

## Author

**Agrima Bhatnagar**

Quantitative Research | Data Science | Machine Learning | High-Performance Computing
