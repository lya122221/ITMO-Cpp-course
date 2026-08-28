#pragma once

#include "command.hpp"
#include "geo_index.hpp"

namespace kvdb {

class GgetCmd : public Command {
public:
  CommandResult Execute(std::shared_ptr<Storage> storage, std::vector<std::string> args) override {
    if (args.size() != 1) {
      std::cerr << "(error) Invalid arguments count" << std::endl;
      return std::monostate();
    }

    auto res = storage->Get(args[0]);
    if (!res.has_value()) {
      std::cerr << res.error() << std::endl;
      return std::monostate();
    }

    Storage::Value value = res.value();
    if (auto ptr = std::get_if<GeoIndex>(&value)) {
      return *ptr; 
    } else {
      std::cerr << "(error) The type associated with this key is not a geo index" << std::endl;
      return std::monostate();
    }
  }
};

class GeoaddCmd : public Command {
public:
  CommandResult Execute(std::shared_ptr<Storage> storage, std::vector<std::string> args) override {
    if (args.size() < 4 || (args.size() - 1) % 3 != 0) {
      std::cerr << "(error) Invalid arguments count for GEOADD" << std::endl;
      return std::monostate();
    }

    Storage::Value value;
    auto res = storage->Get(args[0]);
    if (!res.has_value()) {
      value = GeoIndex();
    } else {
      value = res.value();
    }

    if (auto ptr = std::get_if<GeoIndex>(&value)) {
      int added = 0;
      for (size_t i = 1; i < args.size(); i += 3) {
        try {
          double lon = std::stod(args[i]);
          double lat = std::stod(args[i+1]);
          std::string member = args[i+2];
          ptr->AddPoint(member, lon, lat);
          added++;
        } catch (...) {
          std::cerr << "(error) Invalid coordinates format" << std::endl;
          return std::monostate();
        }
      }

      storage->Set(args[0], *ptr);
      
      return added;
    } else {
      std::cerr << "(error) WRONGTYPE Operation against a key holding the wrong kind of value" << std::endl;
      return std::monostate();
    }
  }
};

class GeodistCmd : public Command {
public:
  CommandResult Execute(std::shared_ptr<Storage> storage, std::vector<std::string> args) override {
    if (args.size() < 3 || args.size() > 4) {
      std::cerr << "(error) Invalid arguments count" << std::endl;
      return std::monostate();
    }

    auto res = storage->Get(args[0]);
    if (!res.has_value()) {
      std::cerr << res.error() << std::endl;
      return std::monostate();
    }

    if (auto ptr = std::get_if<GeoIndex>(&res.value())) {
      std::string unit = (args.size() == 4) ? args[3] : "m";
      double dist = ptr->CalculateDistance(args[1], args[2], unit);
      
      if (dist < 0) {
        return std::monostate();
      }
      
      return std::to_string(dist);
    }
    return std::monostate();
  }
};

class GeosearchCmd : public Command {
public:
  CommandResult Execute(std::shared_ptr<Storage> storage, std::vector<std::string> args) override {
    if (args.size() < 7) {
      std::cerr << "(error) Invalid syntax for GEOSEARCH" << std::endl;
      return std::monostate();
    }

    auto res = storage->Get(args[0]);
    if (!res.has_value()) {
      return std::monostate();
    }

    if (auto ptr = std::get_if<GeoIndex>(&res.value())) {
      double lon = std::stod(args[2]);
      double lat = std::stod(args[3]);
      double radius = std::stod(args[5]);
      std::string unit = args[6];

      bool asc = (std::find(args.begin(), args.end(), "ASC") != args.end());

      return ptr->SearchRadius(lon, lat, radius, unit, asc);
    }

    return std::monostate();
  }
};

class GeosearchstoreCmd : public Command {
public:
  CommandResult Execute(std::shared_ptr<Storage> storage, std::vector<std::string> args) override {
    if (args.size() < 8) {
      std::cerr << "(error) Invalid syntax for GEOSEARCHSTORE" << std::endl;
      return std::monostate();
    }

    std::string dest_key = args[0];
    std::string source_key = args[1];

    auto res = storage->Get(source_key);
    if (!res.has_value()) {
      return 0;
    }

    if (auto ptr = std::get_if<GeoIndex>(&res.value())) {
      try {
        double lon = std::stod(args[3]);
        double lat = std::stod(args[4]);
        double radius = std::stod(args[6]);
        std::string unit = args[7];
        
        bool asc = false;
        int count = -1;

        for (size_t i = 8; i < args.size(); i++) {
          std::string arg = args[i];

          std::transform(arg.begin(), arg.end(), arg.begin(), [](unsigned char c){ return std::toupper(c); });
          
          if (arg == "ASC") {
            asc = true;
          } else if (arg == "COUNT" && i + 1 < args.size()) {
            count = std::stoi(args[i + 1]);
            i++;
          }
        }

        std::vector<std::string> search_results = ptr->SearchRadius(lon, lat, radius, unit, asc);
        if (count > 0 && search_results.size() > count) {
          search_results.resize(count);
        }

        std::list<std::string> result_list(search_results.begin(), search_results.end());
        storage->Set(dest_key, result_list);

        return static_cast<int>(result_list.size());
      } catch (...) {
        std::cerr << "(error) Invalid numeric argument" << std::endl;
        return std::monostate();
      }
    } else {
      std::cerr << "(error) WRONGTYPE Operation against a key holding the wrong kind of value" << std::endl;
      return std::monostate();
    }
  }
};

}