#include "requests_handler.h"
#include <iostream>

// TODO: validate request
Request RequestsHandler::get_request_() {
  std::string from_code;
  std::cout << "Enter the departure city code. For St. Petersburg enter \"c2\": ";
  std::cin >> from_code;

  std::string to_code;
  std::cout << "Enter the arrival city code: ";
  std::cin >> to_code;

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