import pandas as pd
import numpy as np
from pathlib import Path

INPUT = Path("data/processed/aapl_alpha_predictions.csv")
OUTPUT = Path("data/processed/backtest_results.csv")

df = pd.read_csv(INPUT)

# Trading signal
df["signal"] = np.where(
    df["alpha_probability"] > 0.55,
    1,
    np.where(
        df["alpha_probability"] < 0.45,
        -1,
        0
    )
)

# Strategy return
df["strategy_return"] = (
    df["signal"] * df["future_return_10"]
)

# Transaction cost
COST = 0.00005

df["trade"] = (
    df["signal"]
    .diff()
    .abs()
    .fillna(0)
)

df["net_return"] = (
    df["strategy_return"]
    - df["trade"] * COST
)

# Equity curve
df["equity"] = (
    1.0 + df["net_return"]
).cumprod()

# Metrics
total_return = df["equity"].iloc[-1] - 1

vol = df["net_return"].std()

sharpe = (
    df["net_return"].mean() / vol * np.sqrt(len(df))
    if vol > 0
    else 0
)

running_max = df["equity"].cummax()

drawdown = (
    df["equity"] / running_max - 1
)

max_drawdown = drawdown.min()

trades = int(
    (df["trade"] > 0).sum()
)

active = df["signal"] != 0

win_rate = (
    (df.loc[active, "net_return"] > 0).mean()
    if active.any()
    else 0
)

print("=" * 60)
print("             HFT ALPHA BACKTEST")
print("=" * 60)

print(f"\nObservations:       {len(df):,}")
print(f"Trades:             {trades:,}")
print(f"Total return:       {total_return:.4%}")
print(f"Sharpe:             {sharpe:.4f}")
print(f"Max drawdown:       {max_drawdown:.4%}")
print(f"Win rate:           {win_rate:.4%}")

print("\nSignal distribution:")
print(df["signal"].value_counts().sort_index())

OUTPUT.parent.mkdir(
    parents=True,
    exist_ok=True
)

df.to_csv(
    OUTPUT,
    index=False
)

print("\nSaved:")
print(OUTPUT)

print("=" * 60)