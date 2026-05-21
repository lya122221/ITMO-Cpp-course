#include <iostream>
#include <unordered_set>

#include "command.hpp"

namespace kvdb {

class SgetCmd : public Command {
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
    if (auto ptr = std::get_if<std::unordered_set<std::string>>(&value)) {
      return *ptr;
    } else {
      std::cerr << "(error) The type associated with this key is not a set" << std::endl;
      return std::monostate();
    }
  }
};


class SaddCmd : public Command {
public:
  CommandResult Execute(std::shared_ptr<Storage> storage, std::vector<std::string> args) override {
    if (args.size() < 2) {
      std::cerr << "(error) Invalid arguments count" << std::endl;
      return std::monostate();
    }

    std::expected<Storage::Value, std::string> res = storage->Get(args[0]);
    if (!res.has_value()) {
      std::cerr << res.error() << std::endl;
      return std::monostate();
    }

    Storage::Value value = res.value();
    if (auto ptr = std::get_if<std::unordered_set<std::string>>(&value)) {
      for (auto it = ++(args.begin()); it != args.end(); it++) {
        ptr->insert(*it);
      }
      storage->Set(args[0], std::move(*ptr));
      return std::monostate();
    } else {
      std::cerr << "(error) The type associated with this key is not a set" << std::endl;
      return std::monostate();
    }
  }
};

class SremCmd : public Command {
public:
  CommandResult Execute(std::shared_ptr<Storage> storage, std::vector<std::string> args) override {
    if (args.size() < 2) {
      std::cerr << "(error) Invalid arguments count" << std::endl;
      return std::monostate();
    }

    std::expected<Storage::Value, std::string> res = storage->Get(args[0]);
    if (!res.has_value()) {
      std::cerr << res.error() << std::endl;
      return std::monostate();
    }

    Storage::Value value = res.value();
    if (auto ptr = std::get_if<std::unordered_set<std::string>>(&value)) {
      for (auto it = ++(args.begin()); it != args.end(); it++) {
        ptr->erase(*it);
      }
      storage->Set(args[0], std::move(*ptr));
      return std::monostate();
    } else {
      std::cerr << "(error) The type associated with this key is not a set" << std::endl;
      return std::monostate();
    }
  }
};

class SismemberCmd : public Command {
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
    if (auto ptr = std::get_if<std::unordered_set<std::string>>(&value)) {
      auto it = ptr->find(args[1]);
      if (it != ptr->end()) {
        return true;
      } else {
        return false;
      }
      
      return std::monostate();
    } else {
      std::cerr << "(error) The type associated with this key is not a set" << std::endl;
      return std::monostate();
    }
  }
};

class SmembersCmd : public Command {
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
    if (auto ptr = std::get_if<std::unordered_set<std::string>>(&value)) {
      return std::vector<std::string>(ptr->begin(), ptr->end());
    } else {
      std::cerr << "(error) The type associated with this key is not a set" << std::endl;
      return std::monostate();
    }
  }
};

class ScardCmd : public Command {
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
    if (auto ptr = std::get_if<std::unordered_set<std::string>>(&value)) {
      return static_cast<int>(ptr->size());
    } else {
      std::cerr << "(error) The type associated with this key is not a set" << std::endl;
      return std::monostate();
    }
  }
};

class SunionCmd : public Command {
public:
  CommandResult Execute(std::shared_ptr<Storage> storage, std::vector<std::string> args) override {
    if (args.size() < 1) {
      std::cerr << "(error) Invalid arguments count" << std::endl;
      return std::monostate();
    }

    std::unordered_set<std::string> union_set;
    for (const std::string& key : args) {
      std::expected<Storage::Value, std::string> res = storage->Get(key);
      if (!res.has_value()) {
        std::cerr << res.error() << std::endl;
        return std::monostate();
      }

      Storage::Value value = res.value();
      if (auto ptr = std::get_if<std::unordered_set<std::string>>(&value)) {
        union_set.insert(ptr->begin(), ptr->end());
      } else {
        std::cerr << "(error) The type associated with this key is not a set" << std::endl;
        return std::monostate();
      }
    }

    return std::move(union_set);
  }
};

class SinterCmd : public Command {
public:
  CommandResult Execute(std::shared_ptr<Storage> storage, std::vector<std::string> args) override {
    if (args.size() < 1) {
      std::cerr << "(error) Invalid arguments count" << std::endl;
      return std::monostate();
    }

    std::unordered_set<std::string> inter_set;
    bool first = true;
    for (const std::string& key : args) {
      std::expected<Storage::Value, std::string> res = storage->Get(key);
      if (!res.has_value()) {
        std::cerr << res.error() << std::endl;
        return std::monostate();
      }

      Storage::Value value = res.value();
      if (auto ptr = std::get_if<std::unordered_set<std::string>>(&value)) {
        if (first) {
          inter_set = std::move(*ptr);
        } else {
          for (const auto& el : inter_set) {
            std::erase_if(inter_set, [&ptr](const std::string& el) {
              return !ptr->contains(el);
            });
          }
        }
      } else {
        std::cerr << "(error) The type associated with this key is not a set" << std::endl;
        return std::monostate();
      }
    }

    return std::move(inter_set);
  }
};

class SdiffCmd : public Command {
public:
  CommandResult Execute(std::shared_ptr<Storage> storage, std::vector<std::string> args) override {
    if (args.size() < 1) {
      std::cerr << "(error) Invalid arguments count" << std::endl;
      return std::monostate();
    }

    std::unordered_set<std::string> inter_set;
    bool first = true;
    for (const std::string& key : args) {
      std::expected<Storage::Value, std::string> res = storage->Get(key);
      if (!res.has_value()) {
        std::cerr << res.error() << std::endl;
        return std::monostate();
      }

      Storage::Value value = res.value();
      if (auto ptr = std::get_if<std::unordered_set<std::string>>(&value)) {
        if (first) {
          inter_set = std::move(*ptr);
        } else {
          for (const auto& el : inter_set) {
            std::erase_if(inter_set, [&ptr](const std::string& el) {
              return ptr->contains(el);
            });
          }
        }
      } else {
        std::cerr << "(error) The type associated with this key is not a set" << std::endl;
        return std::monostate();
      }
    }

    return std::move(inter_set);
  }
};

class SmoveCmd : public Command {
public:
  CommandResult Execute(std::shared_ptr<Storage> storage, std::vector<std::string> args) override {
    if (args.size() != 3) {
      std::cerr << "(error) Invalid arguments count" << std::endl;
      return std::monostate();
    }

    std::expected<Storage::Value, std::string> source = storage->Get(args[0]);
    if (!source.has_value()) {
      std::cerr << source.error() << std::endl;
      return std::monostate();
    }

    std::expected<Storage::Value, std::string> dest = storage->Get(args[0]);
    if (!dest.has_value()) {
      std::cerr << dest.error() << std::endl;
      return std::monostate();
    }

    Storage::Value value1 = source.value();
    Storage::Value value2 = dest.value();
    auto ptr1 = std::get_if<std::unordered_set<std::string>>(&value1);
    auto ptr2 = std::get_if<std::unordered_set<std::string>>(&value2);
    if (ptr1 && ptr2) {
      auto elem = args[2];

      if (!ptr1->contains(elem)) {
        std::cerr << "(error) Source set does not contain this element" << std::endl;
        return std::monostate();
      }

      ptr1->erase(elem);
      ptr2->insert(elem);
      storage->Set(args[0], *ptr1);
      storage->Set(args[1], *ptr2);
      
      return std::monostate();
    } else {
      std::cerr << "(error) The type associated with this key is not a set" << std::endl;
      return std::monostate();
    }
  }
};

}