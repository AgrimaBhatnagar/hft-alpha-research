import pandas as pd

INPUT = "data/processed/aapl_features.csv"

df = pd.read_csv(INPUT)

print("=" * 50)
print("AAPL DATA QUALITY CHECK")
print("=" * 50)

print(f"\nTotal rows: {len(df):,}")

print("\nMissing values:")
print(df.isna().sum())

print("\nBook state counts:")

print(
    "bid > 0:",
    (df["best_bid"] > 0).sum()
)

print(
    "ask > 0:",
    (df["best_ask"] > 0).sum()
)

print(
    "ask >= bid:",
    (df["best_ask"] >= df["best_bid"]).sum()
)

print(
    "valid spread:",
    (
        (df["best_bid"] > 0) &
        (df["best_ask"] > 0) &
        (df["best_ask"] >= df["best_bid"])
    ).sum()
)

print("\nSpread statistics:")
print(df["spread"].describe())

print("\nFirst valid-looking rows:")
print(
    df[
        (df["best_bid"] > 0) &
        (df["best_ask"] > 0)
    ][[
        "timestamp",
        "best_bid",
        "best_ask",
        "mid_price",
        "spread"
    ]].head(20).to_string(index=False)
)

print("\nLast rows:")
print(
    df[[
        "timestamp",
        "best_bid",
        "best_ask",
        "mid_price",
        "spread"
    ]].tail(10).to_string(index=False)
)