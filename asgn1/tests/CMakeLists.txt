cmake_minimum_required(VERSION 3.10)
project(tank_game VERSION 1.0)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Create position-independent code for shared libraries
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# Add include directory
include_directories(${PROJECT_SOURCE_DIR}/include)

# Add all source files (excluding main.cpp)
file(GLOB SOURCES "${PROJECT_SOURCE_DIR}/src/*.cpp")
list(FILTER SOURCES EXCLUDE REGEX ".*main\\.cpp$")

# Add the main executable
add_executable(tank_game ${SOURCES} "${PROJECT_SOURCE_DIR}/src/main.cpp")

# Add the shared library for Python
add_library(tank_game_lib SHARED
    ${SOURCES}
)

# Set output directory
set_target_properties(tank_game
    PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}"
)

# Set library name and properties
set_target_properties(tank_game_lib
    PROPERTIES
    OUTPUT_NAME "tank_game"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}"
)

# Google Test
include(FetchContent)
FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG release-1.12.1
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

# Enable testing
enable_testing()

# Add tests
add_executable(tank_game_tests
  "tests/position_test.cpp"
  "tests/board_test.cpp"
  "tests/input_parser_test.cpp"
  "tests/integration_test.cpp"
  "tests/game_state_test.cpp"
  "tests/algorithms_test.cpp"
  "tests/game_manager_test.cpp"
  ${SOURCES}
)
target_link_libraries(tank_game_tests gtest_main gmock)

include(GoogleTest)
gtest_discover_tests(tank_game_tests) 