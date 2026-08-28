#include <iostream>
#include <vector>
#include <string>
#include "taskscheduler.h"
#include "model.h"
#include "location.h"
#include "places.h"
#include "weather.h"
#include <nlohmann/json.hpp>
using json = nlohmann::json;

void PrintReport(Weather w, std::vector<Place> places) {
  std::cout << "==========================================\n";
  std::cout << "Погода сейчас: " << w.emoji << " " << w.temp << "C\n";
  std::cout << "Ощущается как: " << w.apparent_temp << "C\n";
  std::cout << (w.is_bad ? "(!) Погода не очень, рекомендую места под крышей.\n" 
                         : "☀️ Отличный день для прогулки!\n");
  
  std::cout << "\nБлижайшие места для вас:\n";
  
  if (places.empty()) {
    std::cout << "[ Ничего не найдено в этом районе ]\n";
  }

  for (const auto& p : places) {
    std::cout << "------------------------------------------\n";
    std::cout << "📍 " << p.name << " [" << p.category << "]\n";
    std::cout << "🏠 " << p.address << "\n";
    std::cout << "🚶 " << p.distance << " метров от вас\n";
  }
  std::cout << "==========================================\n";
}

int main() {
  TTaskScheduler scheduler;

  auto hourTask = scheduler.add(GetCurrentHour);
  auto hourF = hourTask.getFutureResult<int>();

  auto locTask = scheduler.add(GetLocation);
  auto locF = locTask.getFutureResult<Location>();

  auto weatherJsonTask = scheduler.add(GetWeather, locF);
  auto weatherJsonF = weatherJsonTask.getFutureResult<json>();

  auto parseWeatherTask = scheduler.add(ParseWeather, weatherJsonF, hourF);
  auto weatherF = parseWeatherTask.getFutureResult<Weather>();

  auto placesJsonTask = scheduler.add(GetPlaces, locF, weatherF, 3000);
  auto placesJsonF = placesJsonTask.getFutureResult<json>();

  auto parsePlacesTask = scheduler.add(ParsePlaces, placesJsonF);
  auto placesF = parsePlacesTask.getFutureResult<std::vector<Place>>();

  scheduler.add(PrintReport, weatherF, placesF);

  try {
    scheduler.executeAll();
  } catch (const std::exception& e) {
    std::cerr << "\n Error: " << e.what() << std::endl;
  }

  return 0;
}