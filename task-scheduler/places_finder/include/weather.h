#pragma once
#include "model.h"
#include <nlohmann/json.hpp>
using json = nlohmann::json;

json GetWeather(Location location);
Weather ParseWeather(json data, int hour);