# Smart Cube Solver 3D

A web-based 3D Rubik's Cube solver utilizing a blazingly fast C++ solver backend and a Flask/Python web interface. The frontend features a fully interactive 3D cube powered by `cubing.net/js/twisty`.

## 🚀 Features

- **Blazing Fast C++ Engine:** A highly optimized solver written in C++ that generates pruning tables on startup to solve cubes rapidly.
- **Python Flask Backend:** A lightweight server bridging the C++ engine (via `ctypes`) and the web client.
- **Interactive 3D UI:** A slick web interface to input scrambles and watch the cube solve itself in 3D.
- **Docker Support:** Easily containerized and deployable on platforms like Render or Railway.

---

## 🌟 How This Improves Upon Previous CLI Solvers

This project is a massive architectural and user-experience improvement over traditional CLI-based C++ solvers, such as [Sri175/rubikscubesolver](https://github.com/Sri175/rubikscubesolver). 

### Key Improvements:

1. **Interactive 3D Web Interface vs. ASCII Terminal**
   - *Previous:* You had to run a command-line application and interpret text-based ASCII representations of the cube to verify the scramble and solution.
   - *Improvement:* Features a modern, web-based UI with a fully 3D interactive cube. When a solution is calculated, the 3D cube animates the moves directly in your browser.

2. **Language Bridging (C++ to Python via `ctypes`)**
   - *Previous:* Pure C++ implementation that required the user to compile the code locally just to interact with it via the command line.
   - *Improvement:* Keeps the performance-critical solving logic in C++ while exposing it as a shared library (`.dll` or `.so`). The Python Flask server uses `ctypes` to bind to this library, creating a seamless microservice architecture.

3. **Universal Access & Containerization**
   - *Previous:* Difficult to share with non-technical users. They had to install CMake and a C++ compiler to run the executable.
   - *Improvement:* Dockerized out-of-the-box. You can deploy it to any cloud host (like Render, Heroku, or Railway), making it instantly accessible to anyone via a URL, entirely abstracting away the C++ build process for end users.

4. **REST API Ready**
   - *Previous:* The solving capabilities were locked inside the `main.cpp` executable logic.
   - *Improvement:* Exposes a `/solve` REST API endpoint. Other applications can make HTTP POST requests with a scramble and instantly get back the optimal solution sequence.

---

## ⚙️ How It Works Under the Hood

### The C++ Solver Engine: Graph Search & IDA*

At the core of this project is a highly optimized C++ engine that treats the Rubik's Cube as a massive graph traversal problem:

1. **State as a Graph Node:** Every possible configuration of the Rubik's Cube (all 43 quintillion of them!) is represented as a node in a mathematical graph. A single twist of a face (e.g., `U`, `R'`) is an edge connecting one node to another. The solved state is our target node.
2. **IDA* Algorithm:** To find the absolute shortest path from a scrambled state to the solved state, the engine uses **IDA\*** (Iterative Deepening A*). Standard BFS or A* would run out of memory trying to store millions of cube states. IDA* uses a memory-efficient depth-first search approach bounded by an increasing cost threshold, giving us the mathematically shortest path without eating up all your RAM.
3. **Pattern Databases (Heuristics):** IDA* is only fast if it knows roughly how far away it is from the goal. The engine uses **Pattern Databases**—precomputed tables loaded into memory on startup—that perfectly map out the exact number of moves needed to solve specific subsets of the cube (like just the corners). This acts as a powerful heuristic, allowing the algorithm to heavily prune the search tree and instantly skip millions of dead-end paths.

### Full System Architecture

1. **Initialization:** When the Flask server (`app.py`) starts, it loads the compiled C++ shared library using Python's `ctypes`. The C++ engine initializes itself, generating its heuristic Pattern Databases so that solves are near-instantaneous.
2. **User Input:** The user types a scramble sequence (e.g., `R U R' U'`) into the web interface.
3. **API Call:** The frontend JavaScript makes a `POST /solve` request to the Flask server.
4. **C++ Execution:** Flask passes the scramble string to the C++ shared library. The engine applies the scramble, runs the IDA* search against its Pattern Databases, and writes the optimal solution moves into a C-string buffer.
5. **Animation:** Flask reads the string buffer and returns it as a JSON response. The frontend reads the solution and animates it using the `TwistyPlayer` 3D engine.

---

## 🛠️ Project Structure

- `src/` - C++ source files containing the solving logic (`solver.hpp`, `interface.cpp`).
- `app.py` - Flask web server, `ctypes` bindings, and the HTML frontend.
- `CMakeLists.txt` - CMake configuration for building the C++ shared library.
- `Dockerfile` - Docker configuration for containerized deployment.

## 📦 Local Setup

### Prerequisites
- Python 3.x
- CMake
- C++ Compiler (GCC/Clang on Linux/Mac, MSVC on Windows)

### 1. Build the C++ Engine
You need to compile the C++ code into a shared library (`.dll` on Windows, `.so` on Linux/Mac).

```bash
mkdir build
cd build
cmake ..
cmake --build . --config Release
cd ..
```

### 2. Install Python Dependencies
```bash
pip install flask flask-cors
```

### 3. Run the Server
```bash
python app.py
```
Open your browser and navigate to `http://localhost:5003` (or the port specified by your environment) to use the solver.

## 🐳 Docker Setup

You can run the project using Docker without manually setting up CMake or a C++ compiler on your host machine.

```bash
docker build -t smart-cube-solver .
docker run -p 5000:5000 smart-cube-solver
```
The app will be accessible at `http://localhost:5000`.

## 📝 Usage

1. Enter a standard Rubik's Cube scramble (e.g., `R U R' U'`) in the input box.
2. Click **Solve**.
3. The engine will calculate the solution and play it back on the 3D cube visualization!
