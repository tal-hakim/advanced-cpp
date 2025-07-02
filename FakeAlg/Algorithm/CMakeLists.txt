cmake_minimum_required(VERSION 3.28)
project(AlgorithmFake CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

add_compile_options(-fPIC)

# Include directories
include_directories(${CMAKE_SOURCE_DIR}/..)
include_directories(${CMAKE_SOURCE_DIR}/../common)
include_directories(${CMAKE_SOURCE_DIR}/../UserCommon)

# Source files for AlgorithmFake
set(SOURCE_FILES
        FakePlayer.cpp
        FakeTankAlgorithm.cpp
        ConcreteBattleInfo.cpp
        ../UserCommon/utils/DirectionUtils.cpp
        ../UserCommon/utils/MapUtils.cpp
)

# Header files for AlgorithmFake
set(HEADER_FILES
        FakePlayer.h
        FakeTankAlgorithm.h
        ConcreteBattleInfo.h
        ../UserCommon/utils/Direction.h
        ../UserCommon/utils/Position.h
        ../UserCommon/utils/DirectionUtils.h
        ../UserCommon/utils/MapUtils.h
        ../common/Player.h
        ../common/PlayerRegistration.h
)

# Create AlgorithmFake shared library
add_library(${PROJECT_NAME} SHARED ${SOURCE_FILES} ${HEADER_FILES})
target_link_libraries(${PROJECT_NAME})
set_target_properties(${PROJECT_NAME} PROPERTIES PREFIX "" POSITION_INDEPENDENT_CODE ON )

# ---------- AlwaysForward Plugin (.so) ----------
# Source/header for AlwaysForward
set(ALWAYSFORWARD_SRC
        AlwaysForward.cpp
        AlwaysForward.h
)

add_library(AlwaysForward SHARED ${ALWAYSFORWARD_SRC})
target_link_libraries(AlwaysForward)
set_target_properties(AlwaysForward PROPERTIES PREFIX "" POSITION_INDEPENDENT_CODE ON)

# ---------- SpinShooter Plugin (.so) ----------
# Source/header for SpinShooter
set(SPINSHOOTER_SRC
        SpinShooter.cpp
        SpinShooter.h
)

add_library(SpinShooter SHARED ${SPINSHOOTER_SRC})
target_link_libraries(SpinShooter)
set_target_properties(SpinShooter PROPERTIES PREFIX "" POSITION_INDEPENDENT_CODE ON)


# ---------- LookPause Plugin (.so) ----------
# Source/header for SpinShooter
set(LOOKPAUSE_SRC
        LookPause.cpp
        LookPause.h
)

add_library(LookPause SHARED ${LOOKPAUSE_SRC})
target_link_libraries(LookPause)
set_target_properties(LookPause PROPERTIES PREFIX "" POSITION_INDEPENDENT_CODE ON)