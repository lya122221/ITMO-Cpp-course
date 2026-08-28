include(FetchContent)

FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/releases/download/v1.16.0/googletest-1.16.0.tar.gz
  DOWNLOAD_EXTRACT_TIMESTAMP ON
  FIND_PACKAGE_ARGS NAMES GTest
)

set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()

add_executable(
  scheduler-lib-tests
  function_ut.cpp
  future_ut.cpp
  scheduler_ut.cpp
  task_ut.cpp
  model_ut.cpp
  places_ut.cpp
  weather_ut.cpp
  ../places_finder/src/places.cpp
  ../places_finder/src/weather.cpp
  ../places_finder/src/location.cpp
)

target_link_libraries(
  scheduler-lib-tests
  GTest::gtest_main
  GTest::gmock_main
  cpr::cpr
  nlohmann_json::nlohmann_json
)

target_include_directories(scheduler-lib-tests PUBLIC 
  ${PROJECT_SOURCE_DIR}/include 
  ${PROJECT_SOURCE_DIR}/places_finder/src
  ${PROJECT_SOURCE_DIR}/places_finder/include
  ${PROJECT_SOURCE_DIR}
)

include(GoogleTest)

gtest_discover_tests(scheduler-lib-tests)