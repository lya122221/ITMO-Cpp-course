#pragma once
#include <string>
#include <chrono>
#include <vector>

struct Segment {
  std::string transport_type;
  std::string number;
  std::string title;
  std::string carrier;
  std::string vehicle;
  std::string from_station;
  std::string to_station;
  std::string departure;
  std::string arrival;
  double duration;
};

struct Route {
  bool has_transfers = false;
  std::vector<Segment> segments;
  std::string transfer_city;
  double transfer_duration = 0;
  std::string departure;
  std::string arrival;
};

struct Data {
  std::string from_city;
  std::string from_code;
  std::string to_city;
  std::string to_code;
  std::string date;
  std::vector<Route> routes;
  int total = 0;
};