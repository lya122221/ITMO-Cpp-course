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

  Request GetRequest();
  bool ContinueRunningOrNot();
  void PrintResult(const Data& data) const;

  int SafeSecondsForDisplay(double seconds) const;
};