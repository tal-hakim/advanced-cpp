cmake_minimum_required(VERSION 3.28)
project(Algorithm_322213836_212054837 CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

add_compile_options(-fPIC)

# Include directories
include_directories(${CMAKE_SOURCE_DIR}/..)
include_directories(${CMAKE_SOURCE_DIR}/../Common)
include_directories(${CMAKE_SOURCE_DIR}/../UserCommon)

# Source files
set(SOURCE_FILES
    Player_322213836_212054837.cpp
    TankAlgorithm_322213836_212054837.cpp
    ../UserCommon/utils/DirectionUtils.cpp
    ../UserCommon/utils/MapUtils.cpp
)

# Header files
set(HEADER_FILES
    Player_322213836_212054837.h
    TankAlgorithm_322213836_212054837.h
    ../UserCommon/utils/Direction.h
    ../UserCommon/utils/Position.h
    ../UserCommon/utils/DirectionUtils.h
    ../UserCommon/utils/MapUtils.h
    ../Common/Player.h
    ../Common/PlayerRegistration.h
)

# Create shared library
add_library(${PROJECT_NAME} SHARED ${SOURCE_FILES} ${HEADER_FILES})

# Link against UserCommon
target_link_libraries(${PROJECT_NAME})

# Set output name without lib prefix
set_target_properties(${PROJECT_NAME} PROPERTIES PREFIX "" POSITION_INDEPENDENT_CODE ON )