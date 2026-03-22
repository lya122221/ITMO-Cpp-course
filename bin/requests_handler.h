#pragma once
#include "api_client.h"
#include "cache.h"
#include "model.h"
#include <string>

class RequestsHandler {
public:
  RequestsHandler(const Config& config) : api_client_(config.api_key), cache_(config.max_cache_size, config.ttl_seconds) {}
  
  bool HandleRequests();
private:
  APIClient api_client_;
  Cache cache_;
  Request request_;

  Request get_request_();
  bool continue_running_or_not_();
  void print_result_(const Data& data) const;

  int safe_seconds_for_display_(double seconds) const;
};