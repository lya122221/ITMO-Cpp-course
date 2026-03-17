#pragma once
#include "api_client.h"
#include "cache.h"
#include "model.h"
#include <string>

struct Request {
  std::string from_code;
  std::string to_code;
  std::string date;
};

class RequestsHandler {
public:
  void HandleRequests();
private:
  APIClient api_client_;
  Cache cache_;
  Request request_;

  Request get_request_();
  bool continue_running_or_not_();
  void print_result_(const Data& data);
};