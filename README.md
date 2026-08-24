# Smart Cube Solver 3D

A web-based 3D Rubik's Cube solver utilizing a fast C++ solver backend and a Flask/Python web interface. The frontend features a fully interactive 3D cube powered by `cubing.net/js/twisty`.

## 🚀 Features

- **Blazing Fast C++ Engine:** A highly optimized solver written in C++ that generates pruning tables on startup to solve cubes rapidly.
- **Python Flask Backend:** A lightweight server bridging the C++ engine (via `ctypes`) and the web client.
- **Interactive 3D UI:** A slick web interface to input scrambles and watch the cube solve itself in 3D.
- **Docker Support:** Easily containerized and deployable on platforms like Render or Railway.

## 🛠️ Project Structure

- `src/` - C++ source files containing the solving logic (`solver.hpp`, `interface.cpp`).
- `app.py` - Flask web server and the HTML frontend.
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

You can also run the project using Docker without needing to manually set up CMake or a C++ compiler on your host machine.

```bash
docker build -t smart-cube-solver .
docker run -p 5000:5000 smart-cube-solver
```
The app will be accessible at `http://localhost:5000`.

## 📝 Usage

1. Enter a standard Rubik's Cube scramble (e.g., `R U R' U'`) in the input box.
2. Click **Solve**.
3. The engine will calculate the solution and play it back on the 3D cube visualization!
