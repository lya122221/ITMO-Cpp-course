#pragma once
#include <string>
#include <unordered_map>
#include <vector>
#include <cmath>
#include <numbers>
#include <algorithm>
#include <optional>
#include <iostream>

namespace kvdb {

class GeoIndex {
public:
  void AddPoint(const std::string& member, double lon, double lat) {
    points_[member] = {lon, lat};
  }

  std::optional<std::pair<double, double>> GetPoint(const std::string& member) const {
    auto it = points_.find(member);

    if (it != points_.end()) {
      return it->second;
    }

    return std::nullopt;
  }

  double CalculateDistance(const std::string& member1, const std::string& member2, const std::string& unit = "m") const {
    auto p1 = GetPoint(member1);
    auto p2 = GetPoint(member2);
    if (!p1 || !p2) {
      return -1.0;
    }

    double dLat = ToRadians(p2->second - p1->second);
    double dLon = ToRadians(p2->first - p1->first);
    double radLat1 = ToRadians(p1->second);
    double radLat2 = ToRadians(p2->second);

    double a = std::pow(std::sin(dLat / 2.0), 2) + std::pow(std::sin(dLon / 2.0), 2) * std::cos(radLat1) * std::cos(radLat2);
    
    double dist_km = 2.0 * EARTH_RADIUS_KM * std::asin(std::sqrt(a));

    if (unit == "m") {
      return dist_km * 1000.0;
    } else if (unit == "mi") {
      return dist_km / 1.60934;
    } else if (unit == "ft") {
      return dist_km * 3280.84;
    }

    return dist_km;
  }

  std::vector<std::string> SearchRadius(double lon, double lat, double radius, const std::string& unit, bool asc) const {
    double radius_km = radius;
    if (unit == "m") {
      radius_km /= 1000.0;
    } else if (unit == "mi") {
      radius_km *= 1.60934;
    } else if (unit == "ft") {
      radius_km /= 3280.84;
    }

    std::vector<std::pair<std::string, double>> results;

    for (const auto& [member, coords] : points_) {
      double dLat = ToRadians(coords.second - lat);
      double dLon = ToRadians(coords.first - lon);
      double radLat1 = ToRadians(lat);
      double radLat2 = ToRadians(coords.second);

      double a = std::pow(std::sin(dLat / 2.0), 2) + std::pow(std::sin(dLon / 2.0), 2) * std::cos(radLat1) * std::cos(radLat2);
      double dist = 2.0 * EARTH_RADIUS_KM * std::asin(std::sqrt(a));

      if (dist <= radius_km) {
        results.push_back({member, dist});
      }
    }

    if (asc) {
      std::sort(results.begin(), results.end(), [](const auto& a, const auto& b) {
        return a.second < b.second;
      });
    }

    std::vector<std::string> output;
    for (const auto& item : results) {
      output.push_back(item.first);
    }

    return output;
  }

  size_t GetMemoryUsage() const {
    size_t mem = sizeof(*this);
    
    mem += points_.bucket_count() * sizeof(void*); 
    
    for (const auto& [member, coords] : points_) {
      mem += sizeof(void*) * 2;
      mem += sizeof(std::string) + member.capacity();
      mem += sizeof(std::pair<double, double>);
    }

    return mem;
  }

  friend std::ostream& operator<<(std::ostream& os, const GeoIndex& geo) {
    os << "GeoIndex[" << geo.points_.size() << " points]";
    return os;
  }
  
private:
  std::unordered_map<std::string, std::pair<double, double>> points_;
  static constexpr double EARTH_RADIUS_KM = 6372.8;

  static double ToRadians(double degrees) {
    return degrees * std::numbers::pi / 180.0;
  }
};

}