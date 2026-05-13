#pragma once

#include "command.hpp"

namespace kvdb {

class LgetCmd : public Command {
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
    if (auto ptr = std::get_if<std::list<std::string>>(&value)) {
      return *ptr;
    } else {
      std::cerr << "(error) The type associated with this key is not a list" << std::endl;
      return std::monostate();
    }
  }
};

class LpushCmd : public Command {
public:
  CommandResult Execute(std::shared_ptr<Storage> storage, std::vector<std::string> args) override {
    if (args.size() < 2) {
      std::cerr << "(error) Invalid arguments count" << std::endl;
      return std::monostate();
    }

    Storage::Value value;
    std::expected<Storage::Value, std::string> res = storage->Get(args[0]);
    if (!res.has_value()) {
      value = std::list<std::string>();
    } else {
      value = res.value();
    }

    if (auto ptr = std::get_if<std::list<std::string>>(&value)) {
      for (auto it = ++(args.begin()); it != args.end(); it++) {
        ptr->push_front(*it);
      }

      storage->Set(args[0], *ptr);
      return std::monostate();
    } else {
      std::cerr << "(error) The type associated with this key is not a list" << std::endl;
      return std::monostate();
    }
  }
};

class RpushCmd : public Command {
public:
  CommandResult Execute(std::shared_ptr<Storage> storage, std::vector<std::string> args) override {
    if (args.size() < 2) {
      std::cerr << "(error) Invalid arguments count" << std::endl;
      return std::monostate();
    }

    Storage::Value value;
    std::expected<Storage::Value, std::string> res = storage->Get(args[0]);
    if (!res.has_value()) {
      value = std::list<std::string>();
    } else {
      value = res.value();
    }

    if (auto ptr = std::get_if<std::list<std::string>>(&value)) {
      for (auto it = ++(args.begin()); it != args.end(); it++) {
        ptr->push_back(*it);
      }

      storage->Set(args[0], *ptr);
      return std::monostate();
    } else {
      std::cerr << "(error) The type associated with this key is not a list" << std::endl;
      return std::monostate();
    }
  }
};

class LpopCmd : public Command {
public:
  CommandResult Execute(std::shared_ptr<Storage> storage, std::vector<std::string> args) override {
    size_t count = 0;

    if (args.size() == 2) {
      count = std::stoi(args[1]);
    } else if (args.size() == 1) {
      count = 1;
    } else {
      std::cerr << "(error) Invalid arguments count" << std::endl;
      return std::monostate();
    }

    std::expected<Storage::Value, std::string> res = storage->Get(args[0]);
    if (!res.has_value()) {
      std::cerr << res.error() << std::endl;
      return std::monostate();
    }
    
    Storage::Value value = res.value();
    if (auto ptr = std::get_if<std::list<std::string>>(&value)) {
      std::list<std::string> result_list;

      if (ptr->size() < count) {
        std::cerr << "(error) The list does not contain that many elements" << std::endl;
        return std::monostate();
      }

      for (int i = 0; i < count; i++) {
        result_list.push_back(ptr->front());
        ptr->pop_front();
      }

      storage->Set(args[0], *ptr);
      return std::move(result_list);
    } else {
      std::cerr << "(error) The type associated with this key is not a list" << std::endl;
      return std::monostate();
    }
  }
};

class RpopCmd : public Command {
public:
  CommandResult Execute(std::shared_ptr<Storage> storage, std::vector<std::string> args) override {
    size_t count = 0;

    if (args.size() == 2) {
      count = std::stoi(args[1]);
    } else if (args.size() == 1) {
      count = 1;
    } else {
      std::cerr << "(error) Invalid arguments count" << std::endl;
      return std::monostate();
    }

    std::expected<Storage::Value, std::string> res = storage->Get(args[0]);
    if (!res.has_value()) {
      std::cerr << res.error() << std::endl;
      return std::monostate();
    }
    
    Storage::Value value = res.value();
    if (auto ptr = std::get_if<std::list<std::string>>(&value)) {
      std::list<std::string> result_list;

      if (ptr->size() < count) {
        std::cerr << "(error) The list does not contain that many elements" << std::endl;
        return std::monostate();
      }

      for (int i = 0; i < count; i++) {
        result_list.push_front(ptr->back());
        ptr->pop_back();
      }

      storage->Set(args[0], *ptr);
      return std::move(result_list);
    } else {
      std::cerr << "(error) The type associated with this key is not a list" << std::endl;
      return std::monostate();
    }
  }
};

class LlenCmd : public Command {
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
    if (auto ptr = std::get_if<std::list<std::string>>(&value)) {
      return static_cast<int>(ptr->size());
    } else {
      std::cerr << "(error) The type associated with this key is not a list" << std::endl;
      return std::monostate();
    }
  }
};

class LrangeCmd : public Command {
public:
  CommandResult Execute(std::shared_ptr<Storage> storage, std::vector<std::string> args) override {
    if (args.size() != 3) {
      std::cerr << "(error) Invalid arguments count" << std::endl;
      return std::monostate();
    }

    std::expected<Storage::Value, std::string> res = storage->Get(args[0]);
    if (!res.has_value()) {
      std::cerr << res.error() << std::endl;
      return std::monostate();
    }

    Storage::Value value = res.value();
    if (auto ptr = std::get_if<std::list<std::string>>(&value)) {
      int start = std::stoi(args[1]);
      int end = std::stoi(args[2]);
      int range = ptr->size();

      while (start < 0) {
        start += range;
      }
      while (end < 0) {
        end += range;
      }

      if (end > ptr->size()) {
        end = ptr->size();
      }
      start = start % range;
      end = end % range;
      if (start >= end) {
        return std::list<std::string>();
      }

      auto first = std::next(ptr->begin(), start);
      auto last = std::next(ptr->begin(), end);

      return std::list<std::string>(first, last);
    } else {
      std::cerr << "(error) The type associated with this key is not a list" << std::endl;
      return std::monostate();
    }
  }
};

class LindexCmd : public Command {
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
    if (auto ptr = std::get_if<std::list<std::string>>(&value)) {
      int index = std::stoi(args[1]);
      int range = ptr->size();

      while (index < 0) {
        index += range;
      }
      index = index % range;

      auto it = std::next(ptr->begin(), index);

      return *it;
    } else {
      std::cerr << "(error) The type associated with this key is not a list" << std::endl;
      return std::monostate();
    }
  }
};

class LsetCmd : public Command {
public:
  CommandResult Execute(std::shared_ptr<Storage> storage, std::vector<std::string> args) override {
    if (args.size() != 3) {
      std::cerr << "(error) Invalid arguments count" << std::endl;
      return std::monostate();
    }

    std::expected<Storage::Value, std::string> res = storage->Get(args[0]);
    if (!res.has_value()) {
      std::cerr << res.error() << std::endl;
      return std::monostate();
    }

    Storage::Value value = res.value();
    if (auto ptr = std::get_if<std::list<std::string>>(&value)) {
      int index = std::stoi(args[1]);
      int range = ptr->size();

      while (index < 0) {
        index += range;
      }
      index = index % range;

      auto it = std::next(ptr->begin(), index);
      *it = std::move(args[2]);

      return std::monostate();
    } else {
      std::cerr << "(error) The type associated with this key is not a list" << std::endl;
      return std::monostate();
    }
  }
};

class LinsertCmd : public Command {
public:
  CommandResult Execute(std::shared_ptr<Storage> storage, std::vector<std::string> args) override {
    if (args.size() != 4) {
      std::cerr << "(error) Invalid arguments count" << std::endl;
      return std::monostate();
    }

    std::expected<Storage::Value, std::string> res = storage->Get(args[0]);
    if (!res.has_value()) {
      std::cerr << res.error() << std::endl;
      return std::monostate();
    }

    Storage::Value value = res.value();
    if (auto ptr = std::get_if<std::list<std::string>>(&value)) {
      int index = std::stoi(args[2]);
      int range = ptr->size();

      while (index < 0) {
        index += range;
      }
      index = index % range;

      auto it = std::next(ptr->begin(), index);

      std::string cmd = args[1];
      std::transform(cmd.begin(), cmd.end(), cmd.begin(), [](unsigned char c){ return std::tolower(c); });
      if (cmd == "before") {
        it->insert(index, args[3]);
      } else if (cmd == "after") {
        it++;
        it->insert(index, args[3]);
      } else {
        std::cerr << "(error) Invalid argument, use BEFORE or AFTER" << std::endl;
        return std::monostate();
      }

      return std::monostate();
    } else {
      std::cerr << "(error) The type associated with this key is not a list" << std::endl;
      return std::monostate();
    }
  }
};


}