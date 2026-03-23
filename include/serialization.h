#pragma once
#include <nlohmann/json.hpp>
using json = nlohmann::json;
#include "model.h"
#include "cache.h"

inline std::string safe_str(const json& j, const std::string& key) {
    if (j.contains(key) && j[key].is_string()) {
        return j[key].get<std::string>();
    }
    return "";
}

inline double safe_double(const json& j, const std::string& key) {
    if (j.contains(key) && j[key].is_number()) {
        return j[key].get<double>();
    }
    return 0.0;
}

inline int safe_int(const json& j, const std::string& key) {
    if (j.contains(key) && j[key].is_number()) {
        return j[key].get<int>();
    }
    return 0;
}

inline bool safe_bool(const json& j, const std::string& key) {
    if (j.contains(key) && j[key].is_boolean()) {
        return j[key].get<bool>();
    }
    return false;
}

inline void to_json(json& j, const Segment& s) {
    j = json{
        {"transport_type", s.transport_type},
        {"number",         s.number},
        {"title",          s.title},
        {"carrier",        s.carrier},
        {"vehicle",        s.vehicle},
        {"from_station",   s.from_station},
        {"to_station",     s.to_station},
        {"departure",      s.departure},
        {"arrival",        s.arrival},
        {"duration",       s.duration}
    };
}

inline void from_json(const json& j, Segment& s) {
    if (j.contains("thread")) {
        const auto& thread = j["thread"];
        s.transport_type = safe_str(thread, "transport_type");
        s.number         = safe_str(thread, "number");
        s.title          = safe_str(thread, "title");
        s.vehicle        = safe_str(thread, "vehicle");

        if (thread.contains("carrier") && thread["carrier"].is_object()) {
            s.carrier = safe_str(thread["carrier"], "title");
        }

        if (j.contains("from") && j["from"].is_object()) {
            s.from_station = safe_str(j["from"], "title");
        }
        if (j.contains("to") && j["to"].is_object()) {
            s.to_station = safe_str(j["to"], "title");
        }

        s.departure = safe_str(j, "departure");
        s.arrival   = safe_str(j, "arrival");
        s.duration  = safe_double(j, "duration");

    } else {
        s.transport_type = safe_str(j, "transport_type");
        s.number         = safe_str(j, "number");
        s.title          = safe_str(j, "title");
        s.carrier        = safe_str(j, "carrier");
        s.vehicle        = safe_str(j, "vehicle");
        s.from_station   = safe_str(j, "from_station");
        s.to_station     = safe_str(j, "to_station");
        s.departure      = safe_str(j, "departure");
        s.arrival        = safe_str(j, "arrival");
        s.duration       = safe_double(j, "duration");
    }
}

inline void to_json(json& j, const Route& r) {
    j = json{
        {"has_transfers",     r.has_transfers},
        {"segments",          r.segments},
        {"transfer_city",     r.transfer_city},
        {"transfer_duration", r.transfer_duration},
        {"departure",         r.departure},
        {"arrival",           r.arrival}
    };
}

inline void from_json(const json& j, Route& r) {
    r.has_transfers = safe_bool(j, "has_transfers");
    r.departure     = safe_str(j, "departure");
    r.arrival       = safe_str(j, "arrival");

    if (j.contains("thread") || j.contains("details")) {
        r.transfer_city     = "";
        r.transfer_duration = 0.0;

        if (!r.has_transfers) {
            Segment s = j.get<Segment>();
            r.segments.push_back(s);
        } else {
            if (j.contains("transfers") && j["transfers"].is_array()) {
                for (const auto& t : j["transfers"]) {
                    r.transfer_city = safe_str(t, "title");
                }
            }

            if (j.contains("details") && j["details"].is_array()) {
                for (const auto& detail : j["details"]) {
                    if (safe_bool(detail, "is_transfer")) {
                        r.transfer_duration += safe_double(detail, "duration");
                    } else {
                        r.segments.push_back(detail.get<Segment>());
                    }
                }
            }
        }

    } else {
        r.transfer_city     = safe_str(j, "transfer_city");
        r.transfer_duration = safe_double(j, "transfer_duration");

        if (j.contains("segments") && j["segments"].is_array()) {
            r.segments = j["segments"].get<std::vector<Segment>>();
        }
    }
}


inline void to_json(json& j, const Data& d) {
    j = json{
        {"from_city", d.from_city},
        {"from_code", d.from_code},
        {"to_city",   d.to_city},
        {"to_code",   d.to_code},
        {"date",      d.date},
        {"routes",    d.routes},
        {"total",     d.total}
    };
}

inline void from_json(const json& j, Data& d) {
    if (j.contains("search")) {
        const auto& search = j["search"];

        if (search.contains("from") && search["from"].is_object()) {
            d.from_city = safe_str(search["from"], "title");
            d.from_code = safe_str(search["from"], "code");
        }
        if (search.contains("to") && search["to"].is_object()) {
            d.to_city = safe_str(search["to"], "title");
            d.to_code = safe_str(search["to"], "code");
        }
        d.date = safe_str(search, "date");

        if (j.contains("pagination") && j["pagination"].is_object()) {
            d.total = safe_int(j["pagination"], "total");
        }

        if (j.contains("segments") && j["segments"].is_array()) {
            d.routes = j["segments"].get<std::vector<Route>>();
        }

    } else {
        d.from_city = safe_str(j, "from_city");
        d.from_code = safe_str(j, "from_code");
        d.to_city   = safe_str(j, "to_city");
        d.to_code   = safe_str(j, "to_code");
        d.date      = safe_str(j, "date");
        d.total     = safe_int(j, "total");

        if (j.contains("routes") && j["routes"].is_array()) {
            d.routes = j["routes"].get<std::vector<Route>>();
        }
    }
}

inline void to_json(json& j, const CacheEntry& e) {
    j = json{
        {"key",       e.key},
        {"data",      e.data},
        {"timestamp", e.timestamp}
    };
}

inline void from_json(const json& j, CacheEntry& e) {
    e.key       = safe_str(j, "key");
    e.timestamp = j.contains("timestamp") && j["timestamp"].is_number()
                  ? j["timestamp"].get<int64_t>()
                  : 0;

    if (j.contains("data")) {
        e.data = j["data"].get<Data>();
    } else {
        e.data = Data{};
    }
}