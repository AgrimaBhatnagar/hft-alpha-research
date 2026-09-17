import pandas as pd
import numpy as np
from pathlib import Path

from sklearn.metrics import (
    accuracy_score,
    roc_auc_score,
    classification_report
)
from sklearn.ensemble import HistGradientBoostingClassifier
from sklearn.inspection import permutation_importance

INPUT = Path("data/processed/aapl_features.csv")

print("=" * 60)
print("             HFT ALPHA MODEL")
print("=" * 60)

df = pd.read_csv(INPUT)

# Keep event-flow features that do not depend on
# the currently inaccurate reconstructed L1 book.
features = [
    "event_type",
    "side",
    "price",
    "quantity",
    "buy_volume",
    "sell_volume",
    "signed_volume",
    "global_ofi",
]

# Future price proxy:
# use event price shifted forward as a simple first-pass target.
# We will replace this with validated mid-price labels later.
for h in [10, 50, 100]:
    df[f"future_price_{h}"] = df["price"].shift(-h)

df["future_return_10"] = (
    df["future_price_10"] / df["price"] - 1.0
)

# Directional target
df["target"] = np.where(
    df["future_return_10"] > 0,
    1,
    0
)

df = df.dropna(
    subset=["future_return_10"]
).copy()

X = df[features].replace(
    [np.inf, -np.inf],
    np.nan
).fillna(0)

y = df["target"]

# Chronological split
split = int(len(df) * 0.80)

X_train = X.iloc[:split]
X_test = X.iloc[split:]

y_train = y.iloc[:split]
y_test = y.iloc[split:]

print(f"\nTotal samples: {len(df):,}")
print(f"Training:      {len(X_train):,}")
print(f"Testing:       {len(X_test):,}")

model = HistGradientBoostingClassifier(
    max_iter=200,
    learning_rate=0.05,
    max_leaf_nodes=15,
    random_state=42
)

print("\nTraining model...")

model.fit(X_train, y_train)

prob = model.predict_proba(X_test)[:, 1]
pred = (prob >= 0.5).astype(int)

accuracy = accuracy_score(
    y_test,
    pred
)

auc = roc_auc_score(
    y_test,
    prob
)

print("\nMODEL RESULTS")
print("-" * 60)
print(f"Accuracy:       {accuracy:.4f}")
print(f"ROC-AUC:        {auc:.4f}")

print("\nClassification report:")
print(
    classification_report(
        y_test,
        pred,
        digits=4
    )
)

print("\nFEATURE IMPORTANCE")
print("-" * 60)

importance = permutation_importance(
    model,
    X_test,
    y_test,
    n_repeats=5,
    random_state=42
)

ranking = sorted(
    zip(
        features,
        importance.importances_mean
    ),
    key=lambda x: x[1],
    reverse=True
)

for name, value in ranking:
    print(f"{name:20s} {value:.6f}")

# Save predictions
result = df.iloc[split:].copy()

result["alpha_probability"] = prob
result["prediction"] = pred

output = Path(
    "data/processed/aapl_alpha_predictions.csv"
)

result.to_csv(
    output,
    index=False
)

print("\nSaved:")
print(output)

print("=" * 60)