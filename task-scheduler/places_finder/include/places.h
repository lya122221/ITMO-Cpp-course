#pragma once
#include <nlohmann/json.hpp>
using json = nlohmann::json;
#include <vector>
#include "model.h"

json GetPlaces(const Location& location, const Weather& weather, int radius_m);
std::vector<Place> ParsePlaces(json j);