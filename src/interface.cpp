#include "solver.hpp"
#include <cstring>
#include <iostream>

// --- WINDOWS EXPORT MACRO ---
#ifdef _WIN32
    #define DLLEXPORT __declspec(dllexport)
#else
    #define DLLEXPORT
#endif

extern "C" {
    
    // Initialize the global lookup tables (only needs to happen once)
    DLLEXPORT void init_solver() {
        build_tables();
    }

    // Create a NEW Solver instance for every request (Thread Safe)
    DLLEXPORT void solve_scramble(const char* scramble_str, char* output_buffer) {
        try {
            // 1. Create a fresh solver instance
            Solver solver; 
            
            std::string scramble(scramble_str);
            
            // 2. Call the class method .solve()
            std::string solution = solver.solve(scramble);
            
            if (solution.empty()) {
                 strcpy(output_buffer, "Already Solved");
            } else {
                 strcpy(output_buffer, solution.c_str());
            }
        } catch (...) {
            strcpy(output_buffer, "Error in Solver");
        }
    }
}