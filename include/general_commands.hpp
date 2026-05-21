#pragma once
#include <memory>
#include <iostream>
#include <vector>
#include <variant>

#include "command.hpp"
#include "geo_index.hpp"

namespace kvdb {

class SetCmd : public Command {
public:
  CommandResult Execute(std::shared_ptr<Storage> storage, std::vector<std::string> args) override {
    if (args.size() != 2) {
      std::cerr << "(error) Invalid arguments count" << std::endl;
    }

    storage->Set(args[0], args[1]);
    return std::monostate();
  }
};

class ExpireCmd : public Command {
public:
  CommandResult Execute(std::shared_ptr<Storage> storage, std::vector<std::string> args) override {
    if (args.size() != 2) {
      std::cerr << "(error) Invalid arguments count" << std::endl;
      return std::monostate();
    }

    std::expected<Storage::Value, std::string> res = storage->Get(args[0]);
    if (!res.has_value()) {
      std::cerr << res.error() << std::endl;
      return std::monostate();
    }

    Storage::Value value = res.value();
    if (auto ptr = std::get_if<std::string>(&value)) {
      storage->Set(args[0], *ptr, std::chrono::steady_clock::now() + std::chrono::seconds(std::stoi(args[1])));
      return std::monostate();
    } else {
      std::cerr << "(error) The type associated with this key is not a string" << std::endl;
      return std::monostate();
    }
  }
};

class TtlCmd : public Command {
public:
  CommandResult Execute(std::shared_ptr<Storage> storage, std::vector<std::string> args) override {
    if (args.size() != 1) {
      std::cerr << "(error) Invalid arguments count" << std::endl;
      return std::monostate();
    }

    auto ttl = storage->GetTtl(args[0]);
    int seconds = ttl.count();
    return seconds;
  }
};

template<class... Ts> struct overloaded : Ts... { using Ts::operator()...; };
template<class... Ts> overloaded(Ts...) -> overloaded<Ts...>;

class TypeCmd : public Command {
public:
  CommandResult Execute(std::shared_ptr<Storage> storage, std::vector<std::string> args) override {
    if (args.size() != 1) {
      std::cerr << "(error) Invalid arguments count" << std::endl;
      return std::monostate();
    }

    std::expected<Storage::Value, std::string> res = storage->Get(args[0]);
    if (!res.has_value()) {
      return std::string("none");
    }

    Storage::Value value = res.value();
    std::string type_name = std::visit(overloaded {
      [](const std::string&) { return std::string("string"); },
      [](const std::list<std::string>&) { return std::string("list"); },
      [](const std::unordered_set<std::string>&) { return std::string("set"); },
      [](const GeoIndex&) { return std::string("geo"); }
    }, value);

    return type_name;
  }
};

class DelCmd : public Command {
public:
  CommandResult Execute(std::shared_ptr<Storage> storage, std::vector<std::string> args) override {
    if (args.size() < 1) {
      std::cerr << "(error) Invalid arguments count" << std::endl;
      return std::monostate();
    }

    for (const auto& key : args) {
      if (!storage->Del(key)) {
        std::cerr << "(error) Invalid key: " << key << std::endl;
        return std::monostate();
      }
    }
    return std::monostate();
  }
};

class ExistsCmd : public Command {
public:
  CommandResult Execute(std::shared_ptr<Storage> storage, std::vector<std::string> args) override {
    if (args.size() < 1) {
      std::cerr << "(error) Invalid arguments count" << std::endl;
      return std::monostate();
    }

    std::vector<std::string> not_exist;
    for (const auto& key : args) {
      if (!storage->IsExist(key)) {
        not_exist.push_back(std::move(key));
      }
    }

    if (not_exist.empty()) {
      return true;
    }

    return not_exist;
  }
};

class KeysCmd : public Command {
public:
  CommandResult Execute(std::shared_ptr<Storage> storage, std::vector<std::string> args) override {
    if (args.size() != 1) {
      std::cerr << "(error) Invalid arguments count" << std::endl;
      return std::monostate();
    }

    return storage->GetKeysByPattern(args[0]);
  }
};

class FlushdbCmd : public  Command {
public:
  CommandResult Execute(std::shared_ptr<Storage> storage, std::vector<std::string> args) override {
    storage->Flush();
    return std::monostate();
  }
};

class ConfigCmd : public Command {
public:
  CommandResult Execute(std::shared_ptr<Storage> storage, std::vector<std::string> args) override {
    if (args.size() != 2 && args.size() != 3) {
      std::cerr << "(error) Invalid arguments count" << std::endl;
      return std::monostate();
    }

    std::string cmd = ToLower(args[1]);
    if (cmd == "set" && args[2] == "maxmemory") {
      if (args.size() != 3) {
        std::cerr << "(error) Invalid arguments count" << std::endl;
        return std::monostate();
      }

      storage->SetMaxMem(std::stoi(args[2]));
      return std::monostate();
    } else if (cmd == "get" && args[2] == "maxmemory") {
      if (args.size() != 2) {
        std::cerr << "(error) Invalid arguments count" << std::endl;
        return std::monostate();
      }

      return static_cast<int>(storage->GetMaxMem());
    }
  }
private:
  std::string ToLower(const std::string& cmd) {
    std::string lower_cmd = cmd;
    std::transform(lower_cmd.begin(), lower_cmd.end(), lower_cmd.begin(), [](unsigned char c){ return std::tolower(c); });
    return lower_cmd;
  }
};

class DbSizeCmd : public Command {
public:
  CommandResult Execute(std::shared_ptr<Storage> storage, std::vector<std::string> args) override {
    return static_cast<int>(storage->GetSize());
  }
};

class MemUsageCmd : public Command {
public:
  CommandResult Execute(std::shared_ptr<Storage> storage, std::vector<std::string> args) override {
    if (args.size() != 2) {
      std::cerr << "(error) Invalid arguments count" << std::endl;
      return std::monostate();
    }

    size_t mem = storage->GetKeyMemoryUsage(args[1]);
    if (mem == 0) {
      return std::monostate();
    }
    
    return static_cast<int>(mem);
  }
};

}