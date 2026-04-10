FetchContent_Declare(googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG v1.14.0
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

include(GoogleTest)

add_executable(run_tests
    test_cache.cpp
    test_requests_handler.cpp
    test_api_client.cpp
)
target_include_directories(run_tests PRIVATE ${CMAKE_SOURCE_DIR}/include ${CMAKE_SOURCE_DIR}/tests)
target_link_libraries(run_tests PRIVATE routes_lib GTest::gtest_main GTest::gmock)

gtest_discover_tests(run_tests)