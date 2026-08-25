import ctypes
import os
import sys

def load_cuber_engine():
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
        
        return cuber
    except Exception as e:
        print(f"Engine Load Error: {e}")
        print("Make sure you built the project! (cd build -> cmake .. -> cmake --build . --config Release)")
        sys.exit(1)

# Singleton instance
cuber_engine = load_cuber_engine()

def solve(scramble: str) -> str:
    out_buffer = ctypes.create_string_buffer(1024)
    cuber_engine.solve_scramble(scramble.encode('utf-8'), out_buffer)
    return out_buffer.value.decode('utf-8').strip()
