cmake_minimum_required(VERSION 3.12)

project(
    labwork7
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 23)

include(FetchContent)

FetchContent_Declare(
    json
    GIT_REPOSITORY https://github.com/nlohmann/json.git
    GIT_TAG v3.11.3
)
FetchContent_MakeAvailable(json)

FetchContent_Declare(
    cpr
    GIT_REPOSITORY https://github.com/libcpr/cpr.git
    GIT_TAG 1.10.5
)
FetchContent_MakeAvailable(cpr)

FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG v1.14.0
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

add_library(routes_lib
    src/api_client.cpp
    src/cache.cpp
    src/requests_handler.cpp
    src/parser.cpp
)

target_include_directories(routes_lib PUBLIC include)

target_link_libraries(routes_lib PUBLIC
    nlohmann_json::nlohmann_json
    cpr::cpr
)

add_executable(routes_finder src/main.cpp)
target_link_libraries(routes_finder PRIVATE routes_lib)

enable_testing()

add_executable(tests
    tests/test_cache.cpp
    tests/test_requests_handler.cpp
)

target_include_directories(tests PRIVATE include tests)

target_link_libraries(tests PRIVATE
    routes_lib
    GTest::gtest_main
    GTest::gmock
)

add_test(NAME AllTests COMMAND tests)

set(GCOVR_ADDITIONAL_ARGS
    --exclude "${CMAKE_BINARY_DIR}/_deps"
    --exclude "*/tests/*"
    --exclude "/usr/*"
    --exclude-throw-branches
    --exclude-unreachable-branches
    --sort-percentage
)

include(tests/coverage.cmake)