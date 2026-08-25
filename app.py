from flask import Flask, request, jsonify, render_template
from flask_cors import CORS
import os
from engine.cuber import solve as engine_solve

app = Flask(__name__)
CORS(app)

@app.route('/')
def index():
    return render_template('index.html')

@app.route('/solve', methods=['POST'])
def solve():
    data = request.json
    scramble = data.get('scramble', '')
    if not scramble: 
        return jsonify({"error": "No scramble"}), 400

    raw_sol = engine_solve(scramble)
    return jsonify({"solution": raw_sol})

if __name__ == '__main__':
    # Get PORT from environment variable (default to 5003 if local)
    port = int(os.environ.get('PORT', 5003))
    # '0.0.0.0' is required for external access on servers
    app.run(host='0.0.0.0', port=port)