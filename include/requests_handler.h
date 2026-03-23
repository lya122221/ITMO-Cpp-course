#pragma once
#include "api_client.h"
#include "cache.h"
#include "model.h"
#include <string>

class RequestsHandler {
public:
  RequestsHandler(IAPIClient& api_client, ICache& cache) : api_client_(api_client), cache_(cache) {}
  
  bool HandleRequests();
private:
  IAPIClient& api_client_;
  ICache& cache_;
  Request request_;

  Request get_request_();
  bool continue_running_or_not_();
  void print_result_(const Data& data) const;

  int safe_seconds_for_display_(double seconds) const;
};