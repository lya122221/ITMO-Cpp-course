#pragma once
#include <unordered_map>
#include <string>
#include <ctime>

const std::string IP_API_KEY = "YOUR_API_KEY_FOR_2IP";
const std::string GEOAPIFY_API_KEY = "YOUR_API_KEY_FOR_GEOAPIFY";

struct Location {
  double lat;
  double lon;
};

struct Weather {
  std::string emoji;
  double temp;
  double apparent_temp;
  bool is_bad;
};

struct Place {
  std::string name;
  std::string address;
  int distance;
  std::string category;
};

inline int GetCurrentHour() {
  std::time_t t = std::time(nullptr);
  std::tm* now = std::localtime(&t);

  int hour = now->tm_hour;

  return hour;
}