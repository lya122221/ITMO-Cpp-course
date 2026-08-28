cmake_minimum_required(VERSION 3.12)
project(labwork9 LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 23)

add_subdirectory(include)

include(FetchContent)

FetchContent_Declare(json
  GIT_REPOSITORY https://github.com/nlohmann/json.git
  GIT_TAG v3.11.3
)
FetchContent_MakeAvailable(json)

FetchContent_Declare(cpr
  GIT_REPOSITORY https://github.com/libcpr/cpr.git
  GIT_TAG 1.10.5
)
FetchContent_MakeAvailable(cpr)

add_subdirectory(places_finder)

enable_testing()
add_subdirectory(tests)