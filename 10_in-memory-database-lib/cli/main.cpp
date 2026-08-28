#include <iostream>
#include <string>
#include <vector>
#include <algorithm>

#include "database.hpp" 
#include "parser.hpp"
#include "printer.hpp"

using namespace kvdb;
using namespace kvdb::cli;

int main(int argc, char** argv) {
  DataBase db;

  for (int i = 1; i < argc; ++i) {
    std::string arg = argv[i];
    if (arg == "--maxmemory" && i + 1 < argc) {
      try {
        size_t max_bytes = ParseMemoryLimit(argv[i + 1]);
        
        db.ExecuteCommand("CONFIG", "SET", "maxmemory", std::to_string(max_bytes));
      } catch (const std::exception& e) {
        std::cerr << "(error) Invalid maxmemory format: " << e.what() << "\n";
        return 1;
      }
      i++;
    }
  }

  std::string line;
  
  // REPL цикл
  while (true) {
      std::cout << "valkey> ";
      if (!std::getline(std::cin, line)) {
          break; // EOF (Ctrl+D)
      }

      if (line.empty()) {
          continue;
      }

      auto tokens = ParseCommandString(line);
      if (tokens.empty()) {
          continue;
      }

      std::string cmd = tokens[0];
      
      // Обработка команды EXIT (регистронезависимо)
      std::string lower_cmd = cmd;
      std::transform(lower_cmd.begin(), lower_cmd.end(), lower_cmd.begin(), 
                      [](unsigned char c){ return std::tolower(c); });
      
      if (lower_cmd == "exit") {
          break;
      }

      // Удаляем саму команду из токенов, оставляем только аргументы
      tokens.erase(tokens.begin()); 

      // Выполняем и печатаем
      CommandResult result = db.ExecuteCommand(cmd, tokens);
      PrintResult(result);
  }

  return 0;
}