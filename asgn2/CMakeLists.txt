cmake_minimum_required(VERSION 3.20)
set(CMAKE_PKG_REDIRECTS_DIR "${CMAKE_BINARY_DIR}/pkgRedirects" CACHE PATH "Relocated pkgRedirects dir")
project(asgn2)

# Compiler flags
add_compile_options(-Wall -Wextra -Werror -pedantic)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Include directories
include_directories(include common)

# Source files
set(SOURCES
    src/game/Logger.cpp
    src/game/GameBoard.cpp
    src/game/GameManager.cpp
    src/objects/GameObject.cpp
    src/objects/Tank.cpp
    src/objects/MovingElement.cpp
    src/algorithms/Evader.cpp
    src/algorithms/Chaser.cpp
    src/algorithms/Algorithm.cpp
    src/game/DirectionUtils.cpp
    src/main.cpp
)

# Add executable
add_executable(asgn2 ${SOURCES})

