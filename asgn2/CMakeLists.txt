cmake_minimum_required(VERSION 3.20)
set(CMAKE_PKG_REDIRECTS_DIR "${CMAKE_BINARY_DIR}/pkgRedirects" CACHE PATH "Relocated pkgRedirects dir")
project(asgn2)
add_compile_options(-Wall -Wextra -Werror -pedantic)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(asgn2 main.cpp
        common/ActionRequest.h
        common/TankAlgorithm.h
        common/BattleInfo.h
        common/SatelliteView.h
        common/Player.h
        common/PlayerFactory.h
        common/TankAlgorithmFactory.h)
