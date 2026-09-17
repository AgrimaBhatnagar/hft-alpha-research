# HFT Alpha Research Engine

### Hawkes Process–Informed Order Flow Detection

A quantitative research system for studying high-frequency limit-order-book dynamics using an event-driven C++ replay engine, market microstructure features, Hawkes-process modeling, machine-learning signals, and historical backtesting.

---

## Research Question

**Do changes in the self- and cross-excitation of order-flow events provide statistically significant and economically useful information about subsequent short-horizon market behavior?**

The project investigates whether temporal dependencies between limit-order-book events can be transformed into predictive signals for short-horizon market movements.

---

## System Architecture

```text
LOBSTER Message Data
        │
        ▼
┌──────────────────────────┐
│ C++ Limit Order Book     │
│ Replay Engine             │
└────────────┬─────────────┘
             │
             ▼
┌──────────────────────────┐
│ Market Microstructure    │
│ Features                  │
│                          │
│ • OBI                    │
│ • OFI                    │
│ • Microprice             │
│ • Spread                 │
│ • Event Flow             │
└────────────┬─────────────┘
             │
             ▼
┌──────────────────────────┐
│ Hawkes Process Modeling  │
│                          │
│ • Self-excitation        │
│ • Cross-excitation       │
│ • Event intensity        │
│ • Branching structure    │
└────────────┬─────────────┘
             │
             ▼
┌──────────────────────────┐
│ Alpha Signal Generation  │
└────────────┬─────────────┘
             │
             ▼
┌──────────────────────────┐
│ Event-Driven Backtest    │
└────────────┬─────────────┘
             │
             ▼
       Performance Analysis
```

---

## Key Components

### 1. High-Performance Order Book Replay

The C++ engine reconstructs the limit order book from event-level market data and processes order-book updates sequentially.

The engine computes market microstructure signals including:

- Order Book Imbalance (OBI)
- Order Flow Imbalance (OFI)
- Microprice
- Bid-ask spread
- Order-flow transitions
- Short-horizon price movements

---

### 2. Hawkes Process Modeling

The research pipeline models temporal clustering and interaction between order-flow events using multivariate Hawkes processes.

The model captures:

- Baseline event intensity
- Self-excitation
- Cross-excitation
- Event-type interaction
- Branching structure

These dynamics are evaluated as potential predictors of subsequent short-horizon market behavior.

---

### 3. Alpha Signal Generation

Hawkes-process features are combined with order-book and order-flow information to generate event-driven predictive signals.

The system tracks signal transitions and evaluates the subsequent market response.

---

### 4. Backtesting

The research pipeline evaluates generated signals using historical market data.

Performance analysis includes:

- Cumulative return
- Maximum drawdown
- Sharpe ratio
- Hit rate
- ROC-AUC
- Classification accuracy
- Signal transition statistics

---

## Experimental Results

Current experimental results include:

| Metric | Result |
|---|---:|
| LOB events processed | 118K+ |
| Signal transitions | 6,472 |
| ROC-AUC | 0.712 |
| Classification accuracy | 62.6% |
| Cumulative backtest return | 44.92% |
| Maximum drawdown | -3.99% |

> These results correspond to the current historical research configuration. They should not be interpreted as evidence of live-trading profitability. Results depend on data selection, model parameters, execution assumptions, transaction costs, and validation methodology.

---

## Repository Structure

```text
hft-alpha-research/
│
├── api/
│   └── app.py
│
├── backend/
│   ├── app/
│   │   ├── engine.py
│   │   └── main.py
│   └── requirements.txt
│
├── cpp/
│   ├── include/
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

## Technology Stack

**C++17 · Python · CMake · NumPy · Pandas · scikit-learn · Hawkes Processes · Limit Order Books · Quantitative Research**

---

## Data

The research uses event-level limit-order-book data from LOBSTER.

Raw market data is **not included in this repository** because of dataset licensing restrictions.

To reproduce the experiments, obtain the appropriate dataset from LOBSTER and place the files in the expected local data directory.

---

## Reproducibility

The research workflow separates:

1. Market-data processing
2. Order-book reconstruction
3. Feature generation
4. Label generation
5. Hawkes-process modeling
6. Alpha-model training
7. Signal generation
8. Backtesting
9. Performance analysis

Configuration and experimental parameters should be kept separate from the core implementation to make experiments reproducible.

---

## Building the C++ Engine

From the `cpp` directory:

```bash
mkdir build
cd build
cmake ..
cmake --build . --config Release
```

---

## Python Environment

Install the required Python dependencies:

```bash
pip install -r backend/requirements.txt
```

Additional research dependencies may be required depending on the selected experiment.

---

## Testing

The C++ implementation contains unit tests for the order-book components under:

```text
cpp/tests/
```

Benchmarks are available under:

```text
cpp/benchmarks/
```

These include performance tests for event processing, order-book replay, feature generation, and latency/throughput measurements.

---

## Research Limitations

This is a quantitative research project rather than a production trading system.

Important considerations include:

- Historical data does not reproduce live execution conditions.
- Transaction costs and market impact can materially affect realized performance.
- Model parameters can introduce overfitting.
- Backtest performance depends on the selected data period and experimental configuration.
- A limited historical sample is insufficient to establish robustness across market regimes.
- Further walk-forward and out-of-sample validation is required.

---

## Future Work

Potential extensions include:

- Larger multi-day datasets
- Walk-forward validation
- Online Hawkes-process estimation
- Nonlinear Hawkes processes
- Regime-dependent excitation matrices
- Queue-position modeling
- Transaction-cost-aware execution
- Cross-asset validation
- GPU-accelerated feature computation
- Live market-data integration

---

## Disclaimer

This repository is intended for educational and quantitative research purposes.

Historical backtest results do not guarantee future performance and should not be interpreted as investment advice.
