from flask import Flask, jsonify, send_from_directory
from flask_cors import CORS

import os

from .engine import ResearchEngine


FRONTEND_DIR = os.path.abspath(
    os.path.join(
        os.path.dirname(__file__),
        "..",
        "..",
        "frontend"
    )
)


app = Flask(__name__)

CORS(app)

engine = ResearchEngine()



@app.get("/")
def index():

    return send_from_directory(
        FRONTEND_DIR,
        "index.html"
    )


@app.get("/api/health")
def health():

    return jsonify({
        "status": "ok",
        "service": "hft-alpha-engine"
    })


@app.get("/api/market")
def market():

    engine.replay_next()

    return jsonify(
        engine.snapshot()
    )


@app.post("/api/reset")
def reset():

    engine.reset()
    engine.data_index = 0

    return jsonify(
        engine.snapshot()
    )


if __name__ == "__main__":

    app.run(
        host="127.0.0.1",
        port=5000,
        debug=True
    )