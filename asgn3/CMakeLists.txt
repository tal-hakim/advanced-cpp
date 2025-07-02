cmake_minimum_required(VERSION 3.28)
project(asgn3 CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
add_compile_options(
        -Wall       # All standard warnings
        -Wextra     # More (extra) warnings
        -Werror     # Treat warnings as errors
        -pedantic   # Enforce standard compliance
)

# Add subdirectories
add_subdirectory(Algorithm)
add_subdirectory(GameManager)
add_subdirectory(Simulator)
