cmake_minimum_required(VERSION 3.28)
project(Algorithm_322213836_212054837 CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Include directories
include_directories(${CMAKE_SOURCE_DIR}/..)
include_directories(${CMAKE_SOURCE_DIR}/../Common)
include_directories(${CMAKE_SOURCE_DIR}/../UserCommon)

# Source files
set(SOURCE_FILES
    Player_322213836_212054837.cpp
    TankAlgorithm_322213836_212054837.cpp
    ${CMAKE_SOURCE_DIR}/UserCommon/utils/DirectionUtils.cpp
    ${CMAKE_SOURCE_DIR}/UserCommon/utils/MapUtils.cpp
)

# Header files
set(HEADER_FILES
    Player_322213836_212054837.h
    TankAlgorithm_322213836_212054837.h
    ${CMAKE_SOURCE_DIR}/UserCommon/utils/Direction.h
    ${CMAKE_SOURCE_DIR}/UserCommon/utils/Position.h
    ${CMAKE_SOURCE_DIR}/UserCommon/utils/DirectionUtils.h
    ${CMAKE_SOURCE_DIR}/UserCommon/utils/MapUtils.h
)

# Create shared library
add_library(${PROJECT_NAME} SHARED ${SOURCE_FILES} ${HEADER_FILES})

# Link against UserCommon
target_link_libraries(${PROJECT_NAME} PRIVATE UserCommon)

# Set output name without lib prefix
set_target_properties(${PROJECT_NAME} PROPERTIES PREFIX "") 