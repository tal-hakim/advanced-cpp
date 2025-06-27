cmake_minimum_required(VERSION 3.28)
project(Simulator CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# Include directories
include_directories(${CMAKE_SOURCE_DIR}/..)
include_directories(${CMAKE_SOURCE_DIR}/../Common)
include_directories(${CMAKE_SOURCE_DIR}/../UserCommon)

# Registration library source files
set(REGISTRATION_SOURCE_FILES
    PlayerRegistration.cpp
    TankAlgorithmRegistration.cpp
    GameManagerRegistration.cpp
    AlgorithmRegistrar.cpp
    GameManagerRegistrar.cpp
)

# Create registration shared library
add_library(SimulatorRegistration SHARED ${REGISTRATION_SOURCE_FILES})
target_link_libraries(SimulatorRegistration PRIVATE dl)
set_target_properties(SimulatorRegistration PROPERTIES
    PREFIX ""
    POSITION_INDEPENDENT_CODE ON
)

# Main simulator source files
set(SIMULATOR_SOURCE_FILES
    ../UserCommon/utils/DirectionUtils.cpp
    Simulator.cpp
    CompetitiveSimulator.cpp
    ComparativeSimulator.cpp
    GameContainer.cpp
    main.cpp
)

# Header files
set(HEADER_FILES
    AlgorithmRegistrar.h
    GameManagerRegistrar.h
    Simulator.h
    ../Common/SatelliteView.h
    ../UserCommon/BoardSatelliteView.h
    CompetitiveSimulator.h
    ComparativeSimulator.h
    GameContainer.h
    BoardInitInfo.h
)

# Create executable
add_executable(${PROJECT_NAME} ${SIMULATOR_SOURCE_FILES} ${HEADER_FILES})

# Link against registration library and other dependencies
target_link_libraries(${PROJECT_NAME} PRIVATE SimulatorRegistration dl pthread)