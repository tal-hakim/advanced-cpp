cmake_minimum_required(VERSION 3.28)
project(Simulator CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Include directories
include_directories(${CMAKE_SOURCE_DIR}/..)
include_directories(${CMAKE_SOURCE_DIR}/../Common)
include_directories(${CMAKE_SOURCE_DIR}/../UserCommon)

# Source files
set(SOURCE_FILES
    AlgorithmRegistrar.cpp
    PlayerRegistration.cpp
    TankAlgorithmRegistration.cpp
    GameManagerRegistration.cpp
    main.cpp)  # Main executable file

# Header files
set(HEADER_FILES
    AlgorithmRegistrar.h
        GameManagerRegistrar.h
        GameManagerRegistrar.cpp
        ../UserCommon/utils/DirectionUtils.cpp
        ../UserCommon/utils/MapUtils.h)

# Create executable
add_executable(${PROJECT_NAME} ${SOURCE_FILES} ${HEADER_FILES})

# Link against UserCommon and dl library for dynamic loading
target_link_libraries(${PROJECT_NAME} PRIVATE UserCommon dl pthread) 