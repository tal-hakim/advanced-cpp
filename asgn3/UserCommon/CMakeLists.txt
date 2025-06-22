cmake_minimum_required(VERSION 3.28)
project(UserCommon CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Include directories
include_directories(${CMAKE_SOURCE_DIR}/..)
include_directories(${CMAKE_SOURCE_DIR}/../Common)

# Source files
set(SOURCE_FILES
        ../GameManager/objects/GameObject.cpp
        ../GameManager/objects/MovingElement.cpp
        ../GameManager/objects/Tank.cpp
    utils/DirectionUtils.cpp
    utils/MapUtils.cpp)

# Header files
set(HEADER_FILES
        ../GameManager/objects/MovingElement.h
        ../GameManager/objects/Shell.h
        ../GameManager/objects/Tank.h
    utils/Direction.h
    utils/DirectionUtils.h
    utils/MapUtils.h
    utils/Position.h)

# Create static library
add_library(${PROJECT_NAME} STATIC ${SOURCE_FILES} ${HEADER_FILES}) 