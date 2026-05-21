cmake_minimum_required(VERSION 3.14)
project(ValkeyDB)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include_directories(include)

add_executable(valkey cli/main.cpp)

include(FetchContent)
FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG release-1.12.1
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()

file(GLOB TEST_SOURCES "tests/*.cpp")

add_executable(run_tests ${TEST_SOURCES})

target_link_libraries(run_tests gtest_main)

include(GoogleTest)
gtest_discover_tests(run_tests)