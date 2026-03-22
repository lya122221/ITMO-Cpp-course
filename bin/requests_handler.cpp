#include "requests_handler.h"
#include <cmath>
#include <iostream>
#include <limits>
#include <optional>

int RequestsHandler::safe_seconds_for_display_(double seconds) const {
  if (!std::isfinite(seconds) || seconds < 0.0) {
    return 0;
  }
  const double kMax = 1e9;
  if (seconds > kMax) {
    return static_cast<int>(kMax);
  }
  return static_cast<int>(seconds);
}

Request RequestsHandler::get_request_() {
  std::string from_city;
  std::cout << "Enter the departure city: ";
  std::cin >> from_city;

  std::string to_city;
  std::cout << "Enter the arrival city: ";
  std::cin >> to_city;

  std::string from_code = api_client_.GetCityCode(from_city);
  std::string to_code = api_client_.GetCityCode(to_city);

  std::string date;
  std::cout << "Enter departure date (format: YYYY-MM-DD): ";
  std::cin >> date;

  return { from_code, to_code, date };
}

bool RequestsHandler::continue_running_or_not_() {
  while (true) {
    std::cout << "Do you want to continue searching for routes? (y/n): ";
    char answer;
    std::cin >> answer;
    std::cout << std::endl;

    if (answer == 'y') {
      return true;
    } else if (answer == 'n') {
      return false;
    }

    std::cout << "Invalid input. Please enter 'y' to continue or 'n' to exit." << std::endl;
  }
}

bool RequestsHandler::HandleRequests() {
  while (true) {
    request_ = get_request_();

    std::optional<Data> data_opt = cache_.Get(request_);
    if (data_opt == std::nullopt) {
      data_opt = api_client_.GetDataFromRequest(request_);
      if (data_opt == std::nullopt) {
        return false;
      }

      Data data = data_opt.value();
      cache_.Put(data);
      print_result_(data);
    } else {
      print_result_(data_opt.value());
    }

    if (!continue_running_or_not_()) {
      return true;
    }
  }
}

void RequestsHandler::print_result_(const Data& data) const {
  std::cout << "══════════════════════════════════════════════\n";
  std::cout << "  " << data.from_city << " → " << data.to_city << "\n";
  std::cout << "  Дата: " << data.date << "\n";
  std::cout << "  Найдено маршрутов: " << data.total << "\n";
  std::cout << "══════════════════════════════════════════════\n\n";

  for (size_t i = 0; i < data.routes.size(); ++i) {
    const Route& route = data.routes[i];

    std::cout << "── Маршрут " << (i + 1) << " ──";
    if (route.has_transfers) {
      std::cout << " [с пересадкой в " << route.transfer_city << "]";
    }
    std::cout << "\n";

    std::cout << "  Отправление: " << route.departure << "\n";
    std::cout << "  Прибытие:    " << route.arrival << "\n";

    if (route.has_transfers) {
      int td = safe_seconds_for_display_(route.transfer_duration);
      int hours = td / 3600;
      int mins = (td % 3600) / 60;
      std::cout << "  Пересадка:   " << hours << "ч " << mins << "мин\n";
    }

    for (size_t s = 0; s < route.segments.size(); ++s) {
      const Segment& seg = route.segments[s];

      std::cout << "\n";
      if (route.segments.size() > 1) {
        std::cout << "    Сегмент " << (s + 1) << ":\n";
      }

      std::cout << "    Рейс:       " << seg.number;
      if (!seg.title.empty()) {
        std::cout << " (" << seg.title << ")";
      }
      std::cout << "\n";

      std::cout << "    Тип:        " << seg.transport_type << "\n";
      std::cout << "    Перевозчик: " << seg.carrier << "\n";

      if (!seg.vehicle.empty()) {
        std::cout << "    Транспорт:  " << seg.vehicle << "\n";
      }

      std::cout << "    Откуда:     " << seg.from_station << "\n";
      std::cout << "    Куда:       " << seg.to_station << "\n";
      std::cout << "    Отправл.:   " << seg.departure << "\n";
      std::cout << "    Прибытие:   " << seg.arrival << "\n";

      int dur = safe_seconds_for_display_(seg.duration);
      int hours = dur / 3600;
      int mins = (dur % 3600) / 60;
      std::cout << "    В пути:     " << hours << "ч " << mins << "мин\n";
    }

    std::cout << "\n";
  }
}