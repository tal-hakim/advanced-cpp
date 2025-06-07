cmake_minimum_required(VERSION 3.20)
set(CMAKE_PKG_REDIRECTS_DIR "${CMAKE_BINARY_DIR}/pkgRedirects" CACHE PATH "Relocated pkgRedirects dir")
project(asgn2)

# Compiler flags
add_compile_options(-Wall -Wextra -Werror -pedantic)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Include directories
include_directories(
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/common
)

# Source files

set(GAME_SOURCES
    src/game/Logger.cpp
    src/game/GameBoard.cpp
    src/game/GameManager.cpp
    src/game/ConcretePlayer.cpp
    src/game/ConcreteBattleInfo.cpp
    src/game/ConcreteTankAlgorithm.cpp
    src/game/DirectionUtils.cpp
        src/game/PlayerOne.cpp
        src/game/PlayerTwo.cpp
        src/game/AlgorithmOne.cpp
)

set(OBJECT_SOURCES
    src/objects/GameObject.cpp
    src/objects/Tank.cpp
    src/objects/MovingElement.cpp
)

# Combine all sources
set(SOURCES
    ${GAME_SOURCES}
    ${OBJECT_SOURCES}
    ${ALGORITHM_SOURCES}
    src/main.cpp
        include/game/AlgorithmOne.h
        src/game/AlgorithmOne.cpp
        include/game/AlgorithmTwo.h
        src/game/AlgorithmTwo.cpp
        src/game/MapUtils.cpp
        include/game/MapUtils.h
        src/game/MapUtils.h
)

# Add executable
add_executable(asgn2 ${SOURCES})

