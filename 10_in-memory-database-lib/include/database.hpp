#pragma once
#include <string>
#include <vector>
#include <functional>
#include <unordered_map>
#include <memory>
#include <algorithm>

#include "command.hpp"
#include "storage.hpp"
#include "string_commands.hpp"
#include "list_commands.hpp"
#include "general_commands.hpp"
#include "set_commands.hpp"
#include "geo_commands.hpp"

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

    RegisterCommand("LGET", std::make_unique<LgetCmd>(LgetCmd()));
    RegisterCommand("LPUSH", std::make_unique<LpushCmd>(LpushCmd()));
    RegisterCommand("RPUSH", std::make_unique<RpushCmd>(RpushCmd()));
    RegisterCommand("LPOP", std::make_unique<LpopCmd>(LpopCmd()));
    RegisterCommand("RPOP", std::make_unique<RpopCmd>(RpopCmd()));
    RegisterCommand("LLEN", std::make_unique<LlenCmd>(LlenCmd()));
    RegisterCommand("LRANGE", std::make_unique<LrangeCmd>(LrangeCmd()));
    RegisterCommand("LINDEX", std::make_unique<LindexCmd>(LindexCmd()));
    RegisterCommand("LSET", std::make_unique<LsetCmd>(LsetCmd()));
    RegisterCommand("LINSERT", std::make_unique<LinsertCmd>(LinsertCmd()));

    RegisterCommand("TYPE", std::make_unique<TypeCmd>(TypeCmd()));
    RegisterCommand("DEL", std::make_unique<DelCmd>(DelCmd()));
    RegisterCommand("EXISTS", std::make_unique<ExistsCmd>(ExistsCmd()));
    RegisterCommand("KEYS", std::make_unique<KeysCmd>(KeysCmd()));
    RegisterCommand("FLUSHDB", std::make_unique<FlushdbCmd>(FlushdbCmd()));
    RegisterCommand("CONFIG", std::make_unique<ConfigCmd>(ConfigCmd()));
    RegisterCommand("DBSIZE", std::make_unique<DbSizeCmd>(DbSizeCmd()));
    RegisterCommand("MEMORY", std::make_unique<MemUsageCmd>(MemUsageCmd()));

    RegisterCommand("SGET", std::make_unique<SgetCmd>(SgetCmd()));
    RegisterCommand("SADD", std::make_unique<SaddCmd>(SaddCmd()));
    RegisterCommand("SREM", std::make_unique<SremCmd>(SremCmd()));
    RegisterCommand("SISMEMBER", std::make_unique<SismemberCmd>(SismemberCmd()));
    RegisterCommand("SMEMBERS", std::make_unique<SmembersCmd>(SmembersCmd()));
    RegisterCommand("SCARD", std::make_unique<ScardCmd>(ScardCmd()));
    RegisterCommand("SUNION", std::make_unique<SunionCmd>(SunionCmd()));
    RegisterCommand("SINTER", std::make_unique<SinterCmd>(SinterCmd()));
    RegisterCommand("SDIFF", std::make_unique<SdiffCmd>(SdiffCmd()));
    RegisterCommand("SMOVE", std::make_unique<SmoveCmd>(SmoveCmd()));

    RegisterCommand("GGET", std::make_unique<GgetCmd>(GgetCmd()));
    RegisterCommand("GEOADD", std::make_unique<GeoaddCmd>(GeoaddCmd()));
    RegisterCommand("GEODIST", std::make_unique<GeodistCmd>(GeodistCmd()));
    RegisterCommand("GEOSEARCH", std::make_unique<GeosearchCmd>(GeosearchCmd()));
    RegisterCommand("GEOSEARCHSTORE", std::make_unique<GeosearchstoreCmd>(GeosearchstoreCmd()));
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