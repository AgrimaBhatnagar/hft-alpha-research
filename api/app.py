from flask import Flask, jsonify
import pandas as pd
from pathlib import Path

app = Flask(__name__)

PREDICTIONS = Path("data/processed/aapl_alpha_predictions.csv")
BACKTEST = Path("data/processed/backtest_results.csv")


@app.get("/api/health")
def health():
    return jsonify({
        "status": "ok",
        "engine": "C++ HFT Engine",
        "market": "AAPL",
        "dataset": "LOBSTER"
    })


@app.get("/api/features")
def features():
    df = pd.read_csv(PREDICTIONS)
    row = df.iloc[-1]

    return jsonify({
        "timestamp": int(row["timestamp"]),
        "price": float(row["price"]),
        "quantity": int(row["quantity"]),
        "signed_volume": float(row["signed_volume"]),
        "global_ofi": float(row["global_ofi"]),
        "alpha_probability": float(row["alpha_probability"]),
        "prediction": int(row["prediction"])
    })


@app.get("/api/predictions")
def predictions():
    df = pd.read_csv(PREDICTIONS)

    return jsonify(
        df.tail(100)[[
            "timestamp",
            "price",
            "signed_volume",
            "global_ofi",
            "alpha_probability",
            "prediction"
        ]].to_dict(orient="records")
    )


@app.get("/api/backtest")
def backtest():
    df = pd.read_csv(BACKTEST)

    equity = df["equity"]

    return jsonify({
        "observations": len(df),
        "final_equity": float(equity.iloc[-1]),
        "total_return": float(equity.iloc[-1] - 1),
        "max_drawdown": float(
            (equity / equity.cummax() - 1).min()
        ),
        "trades": int(
            (df["trade"] > 0).sum()
        )
    })


if __name__ == "__main__":
    app.run(
        host="127.0.0.1",
        port=5000,
        debug=True
    )