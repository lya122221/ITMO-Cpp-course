#pragma once
#include "command.hpp"
#include <expected>

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

class GetCmd : public Command {
public:
  CommandResult Execute(std::shared_ptr<Storage> storage, std::vector<std::string> args) override {
    if (args.size() != 1) {
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
      return *ptr;
    } else {
      std::cerr << "(error) The type associated with this key is not a string" << std::endl;
      return std::monostate();
    }
  }
};

class StrlenCmd : public Command {
public:
  CommandResult Execute(std::shared_ptr<Storage> storage, std::vector<std::string> args) override {
    if (args.size() != 1) {
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
      return static_cast<int>((*ptr).size());
    } else {
      std::cerr << "(error) The type associated with this key is not a string" << std::endl;
      return std::monostate();
    }
  }
};

class AppendCmd : public Command {
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
      storage->Set(args[0], *ptr + args[1]);
      return std::monostate();
    } else {
      std::cerr << "(error) The type associated with this key is not a string" << std::endl;
      return std::monostate();
    }
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

}