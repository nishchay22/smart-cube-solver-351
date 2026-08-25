cmake_minimum_required(VERSION 3.10)
project(SmartCubeSolver)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# Source files
set(SOURCES 
    src/interface.cpp
    src/solver.cpp
)

# Create the shared library (DLL)
add_library(cuber SHARED ${SOURCES})

# Output directory settings
set_target_properties(cuber PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/build"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/build"
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/build"
)