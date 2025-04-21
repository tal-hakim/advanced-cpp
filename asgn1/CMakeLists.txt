cmake_minimum_required(VERSION 3.20)
set(CMAKE_PKG_REDIRECTS_DIR "${CMAKE_BINARY_DIR}/pkgRedirects" CACHE PATH "Relocated pkgRedirects dir")
project(asgn1)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
include_directories(include)
add_executable(asgn1 src/main.cpp
        include/game/Position.h
        include/objects/GameObject.h
        include/game/GameBoard.h
        include/game/GameManager.h
        include/game/Direction.h
        include/objects/Shell.h
        include/objects/Mine.h
        include/objects/Wall.h
        include/objects/Tank.h
        include/game/Logger.h
        src/game/Logger.cpp
        src/game/GameBoard.cpp
        src/game/GameManager.cpp
        include/game/Action.h
        src/objects/GameObject.cpp
        src/objects/Tank.cpp
        include/objects/MovingElement.h
        src/objects/MovingElement.cpp
        include/definitions.h
        include/algorithms/Algorithm.h
        include/algorithms/Chaser.h
        include/algorithms/Evader.h
        src/algorithms/Evader.cpp
        src/algorithms/Chaser.cpp
)
