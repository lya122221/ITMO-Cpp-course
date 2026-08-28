#pragma once
#include "command.hpp"
#include <expected>

namespace kvdb {

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

}