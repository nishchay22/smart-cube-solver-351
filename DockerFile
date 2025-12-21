# 1. Use a Linux Base Image with Python
FROM python:3.9-slim

# 2. Install Build Tools (CMake, G++, Make)
RUN apt-get update && apt-get install -y \
    cmake \
    g++ \
    build-essential \
    && rm -rf /var/lib/apt/lists/*

# 3. Set Working Directory
WORKDIR /app

# 4. Copy Files
COPY . /app

# 5. Install Python Dependencies
RUN pip install flask flask-cors

# 6. Compile C++ Code (The Linux Way)
RUN mkdir -p build && cd build && \
    cmake .. && \
    make

# 7. Expose the Port (Render/Railway use env PORT, usually 5000-10000)
ENV PORT=5000
EXPOSE 5000

# 8. Run the App
CMD ["python", "app.py"]