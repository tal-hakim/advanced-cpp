cmake_minimum_required(VERSION 3.28)
project(GameManager_322213836_212054837 CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# Add visibility flags
add_compile_options(-fvisibility=hidden -fvisibility-inlines-hidden)

# Include directories
include_directories(${CMAKE_SOURCE_DIR}/..)
include_directories(${CMAKE_SOURCE_DIR}/../Common)
include_directories(${CMAKE_SOURCE_DIR}/../UserCommon)

# Source files
set(SOURCE_FILES
        GameManager_322213836_212054837.cpp
    GameBoard.cpp
    Logger.cpp)

# Header files
set(HEADER_FILES
        GameManager_322213836_212054837.h
    GameBoard.h
    Logger.h
        ../UserCommon/utils/definitions.h
        ../Common/PlayerRegistration.h
        ../Common/GameManagerRegistration.h
        ../Common/GameManagerRegistration.h
        ../Common/PlayerRegistration.h
        ../Common/TankAlgorithmRegistration.h
        ../Simulator/TankAlgorithmRegistration.cpp
        ../UserCommon/utils/MapUtils.h
        ../UserCommon/utils/DirectionUtils.cpp
        ../UserCommon/utils/MapUtils.h
        ../UserCommon/utils/MapUtils.cpp
        objects/MovingElement.cpp
        objects/GameObject.cpp
        objects/Wall.h
        objects/Mine.h
        objects/Tank.cpp)

# Create shared library
add_library(${PROJECT_NAME} SHARED ${SOURCE_FILES} ${HEADER_FILES})




# Link against UserCommon
target_link_libraries(${PROJECT_NAME} PRIVATE UserCommon)

# Set output name without lib prefix
set_target_properties(${PROJECT_NAME} PROPERTIES PREFIX "") 