#include <iostream>
#include <string>
#include "api_client.h"
#include "model.h"
#include "requests_handler.h"
#include "parser.h"
#include <filesystem>
#include <fstream>

int main() {
  Parser parser;
  Config config = parser.ParseConfig();

  RequestsHandler handler(config);
  handler.HandleRequests();
  
  return 0;
}