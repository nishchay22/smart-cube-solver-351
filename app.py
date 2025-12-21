from flask import Flask, request, jsonify, make_response
from flask_cors import CORS
import ctypes
import os
import sys

# --- 1. SETUP C++ ENGINE ---
# Ensure we look in the build folder
if os.name == 'nt':
    # Visual Studio typically puts the DLL in build/Release/cuber.dll
    # We check multiple possible locations just in case
    path_release = os.path.abspath("build/Release/cuber.dll")
    path_debug = os.path.abspath("build/Debug/cuber.dll")
    path_flat = os.path.abspath("build/cuber.dll")

    if os.path.exists(path_release):
        lib_path = path_release
    elif os.path.exists(path_debug):
        lib_path = path_debug
    else:
        lib_path = path_flat
        
    print(f"Loading DLL from: {lib_path}")

    # Allow Windows to find dependencies in that folder
    if hasattr(os, 'add_dll_directory'):
        os.add_dll_directory(os.path.dirname(lib_path))
else:
    # Linux/Mac usually output directly to build/libcuber.so
    lib_path = os.path.abspath("build/libcuber.so")

try:
    cuber = ctypes.CDLL(lib_path)
    
    # Interface Setup
    cuber.init_solver.argtypes = [] 
    cuber.init_solver.restype = None
    
    cuber.solve_scramble.argtypes = [ctypes.c_char_p, ctypes.c_char_p]
    cuber.solve_scramble.restype = None
    
    print("Initializing Engine (Generating Pruning Tables)...")
    cuber.init_solver()
    print("Engine Ready!")

except Exception as e:
    print(f"Engine Load Error: {e}")
    print("Make sure you built the project! (cd build -> cmake .. -> cmake --build . --config Release)")
    sys.exit(1)
# --- 2. THE WEBPAGE (The One That Worked) ---
HTML_PAGE = """
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Smart Cube Solver 3D</title>
    <style>
        body { font-family: 'Segoe UI', sans-serif; text-align: center; background: #1a1a1a; color: #fff; margin: 0; padding: 20px; }
        .container { max-width: 700px; margin: 0 auto; background: #2a2a2a; padding: 30px; border-radius: 15px; }
        input { padding: 12px; width: 60%; font-size: 18px; border-radius: 8px; border: 2px solid #444; background: #333; color: white; }
        button { padding: 12px 30px; font-size: 18px; cursor: pointer; background: #00ffcc; color: #000; border: none; border-radius: 8px; margin-left: 10px; }
        #solution-box { margin-top: 20px; font-size: 24px; font-weight: bold; color: #fff; min-height: 40px; }
        #cube-container { width: 100%; height: 400px; margin: 20px auto; display: flex; justify-content: center; align-items: center; border: 1px dashed #444; }
    </style>
</head>
<body>
    <div class="container">
        <h1>🚀 C++ Smart Solver</h1>
        <input type="text" id="scrambleInput" placeholder="Enter Scramble (e.g., R U R' U')">
        <button id="solveBtn">Solve</button>
        
        <div id="cube-container">
            <span id="loading-text">Loading 3D Engine...</span>
        </div>
        
        <div id="solution-box">Ready.</div>
    </div>

    <script type="module">
        import { TwistyPlayer } from "https://cdn.cubing.net/js/cubing/twisty";

        const container = document.getElementById("cube-container");
        document.getElementById("loading-text").remove();

        const player = new TwistyPlayer({
            visualization: "3D",
            controlPanel: "none",
            background: "none",
            alg: ""
        });
        container.appendChild(player);

        document.getElementById("solveBtn").onclick = async function() {
            const scramble = document.getElementById("scrambleInput").value;
            const solutionBox = document.getElementById("solution-box");

            if(!scramble) return;
            solutionBox.innerText = "Calculating...";
            player.experimentalSetupAlg = scramble;

            try {
                const response = await fetch('/solve', {
                    method: 'POST',
                    headers: { 'Content-Type': 'application/json' },
                    body: JSON.stringify({ scramble: scramble })
                });
                const data = await response.json();
                
                if(data.error) {
                    solutionBox.innerText = "Error: " + data.error;
                } else {
                    solutionBox.innerText = data.solution;
                    player.alg = data.solution;
                    player.play();
                }
            } catch (err) {
                solutionBox.innerText = "Error: " + err.message;
            }
        };
    </script>
</body>
</html>
"""

# --- 3. FLASK SERVER ---
app = Flask(__name__)
CORS(app)

@app.route('/')
def index():
    resp = make_response(HTML_PAGE)
    resp.headers['Cache-Control'] = 'no-cache, no-store, must-revalidate'
    return resp

@app.route('/solve', methods=['POST'])
def solve():
    data = request.json
    scramble = data.get('scramble', '')
    if not scramble: return jsonify({"error": "No scramble"}), 400

    out_buffer = ctypes.create_string_buffer(1024)
    cuber.solve_scramble(scramble.encode('utf-8'), out_buffer)
    raw_sol = out_buffer.value.decode('utf-8').strip()
    return jsonify({"solution": raw_sol})

if __name__ == '__main__':
    # Get PORT from environment variable (default to 5003 if local)
    port = int(os.environ.get('PORT', 5003))
    # '0.0.0.0' is required for external access on servers
    app.run(host='0.0.0.0', port=port)