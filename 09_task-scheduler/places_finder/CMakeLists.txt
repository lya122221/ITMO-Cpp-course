add_executable(places_finder 
  main.cpp 
  src/location.cpp 
  src/weather.cpp 
  src/places.cpp
)

target_include_directories(places_finder PRIVATE 
  ${CMAKE_SOURCE_DIR}/include
  ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(places_finder PRIVATE 
  cpr::cpr 
  nlohmann_json::nlohmann_json
)