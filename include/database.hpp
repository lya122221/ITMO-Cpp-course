#pragma once
#include <string>
#include <vector>
#include <functional>
#include <unordered_map>
#include <memory>
#include <algorithm>

#include "command.hpp"
#include "storage.hpp"
#include "string_command.hpp"

namespace kvdb {

class DataBase {
public:
  DataBase() : storage_(std::make_shared<Storage>(Storage())) {
    RegisterCommand("SET", std::make_unique<SetCmd>(SetCmd()));
    RegisterCommand("GET", std::make_unique<GetCmd>(GetCmd()));
    RegisterCommand("STRLEN", std::make_unique<StrlenCmd>(StrlenCmd()));
    RegisterCommand("APPEND", std::make_unique<AppendCmd>(AppendCmd()));
    RegisterCommand("EXPIRE", std::make_unique<ExpireCmd>(ExpireCmd()));
    RegisterCommand("TTL", std::make_unique<TtlCmd>(TtlCmd()));
  }

  CommandResult ExecuteCommand(const std::string& cmd, std::vector<std::string> args) {
    auto cmd_it = commands_.find(cmd);
    if (cmd_it == commands_.end()) {
      std::cerr << "(error) Command not found" << std::endl;
      return std::monostate();
    }

    return cmd_it->second->Execute(storage_, std::move(args));
  }

  template <typename... Args>
  CommandResult ExecuteCommand(const std::string& cmd, Args&&... args) {
    std::string lower_cmd = cmd;
    std::transform(lower_cmd.begin(), lower_cmd.end(), lower_cmd.begin(), [](unsigned char c){ return std::tolower(c); });

    std::vector<std::string> args_vec; 
    auto to_string_helper = [](auto&& arg) -> std::string {
      using ArgType = std::decay_t<decltype(arg)>;
      
      if constexpr (std::is_constructible_v<std::string, ArgType>) {
        return std::string(std::forward<decltype(arg)>(arg));
      } else if constexpr (std::is_arithmetic_v<ArgType>) {
        return std::to_string(std::forward<decltype(arg)>(arg));
      } else {
        std::cerr << "(error) Invalid argument type" << std::endl;
        return "nil";
      }
    };

    (args_vec.push_back(to_string_helper(args)), ...);

    return ExecuteCommand(lower_cmd, std::move(args_vec));
  }

  void RegisterCommand(const std::string& name, std::unique_ptr<Command> cmd) {
    std::string lower_name = name;
    std::transform(lower_name.begin(), lower_name.end(), lower_name.begin(), [](unsigned char c){ return std::tolower(c); });
    commands_[lower_name] = std::move(cmd);
  }
private:
  std::shared_ptr<Storage> storage_;
  std::unordered_map<std::string, std::unique_ptr<Command>> commands_;
};

}