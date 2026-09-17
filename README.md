@'
# HFT Alpha Research Engine

## Hawkes Process–Informed Order Flow Detection

A quantitative research project that models high-frequency limit order book dynamics using C++, Python, Hawkes processes, and event-driven backtesting.

## Highlights

- Event-driven C++ limit order book replay engine
- Order Book Imbalance (OBI), Order Flow Imbalance (OFI), and Microprice features
- Hawkes process modeling for self- and cross-excitation
- Machine learning signal generation in Python
- Historical backtesting and performance evaluation

## Tech Stack

- C++17
- Python
- CMake
- scikit-learn
- NumPy
- Pandas

## Repository Structure

```text
cpp/        High-performance order book engine
python/     Alpha model & backtesting
backend/    API components
frontend/   Simple interface
api/        Application entry points
