#pragma once
#include <nlohmann/json.hpp>
using json = nlohmann::json;
#include "model.h"
#include "cache.h"

inline void to_json(json& j, const Segment& s) {
    j = json{
        {"transport_type", s.transport_type},
        {"number", s.number},
        {"title", s.title},
        {"carrier", s.carrier},
        {"vehicle", s.vehicle},
        {"from_station", s.from_station},
        {"to_station", s.to_station},
        {"departure", s.departure},
        {"arrival", s.arrival},
        {"duration", s.duration}
    };
}

inline void from_json(const json& j, Segment& s) {
    s.transport_type = j.value("transport_type", "");
    s.number         = j.value("number", "");
    s.title          = j.value("title", "");
    s.carrier        = j.value("carrier", "");
    s.vehicle        = j.value("vehicle", "");
    s.from_station   = j.value("from_station", "");
    s.to_station     = j.value("to_station", "");
    s.departure      = j.value("departure", "");
    s.arrival        = j.value("arrival", "");
    s.duration       = j.value("duration", 0.0);
}

inline void to_json(json& j, const Route& r) {
    j = json{
        {"has_transfers", r.has_transfers},
        {"segments", r.segments},
        {"transfer_city", r.transfer_city},
        {"transfer_duration", r.transfer_duration},
        {"departure", r.departure},
        {"arrival", r.arrival}
    };
}

inline void from_json(const json& j, Route& r) {
    r.has_transfers     = j.value("has_transfers", false);
    r.transfer_city     = j.value("transfer_city", "");
    r.transfer_duration = j.value("transfer_duration", 0.0);
    r.departure         = j.value("departure", "");
    r.arrival           = j.value("arrival", "");

    if (j.contains("segments") && j["segments"].is_array()) {
        r.segments = j["segments"].get<std::vector<Segment>>();
    } else {
        r.segments.clear();
    }
}

inline void to_json(json& j, const Data& d) {
    j = json{
        {"from_city", d.from_city},
        {"from_code", d.from_code},
        {"to_city", d.to_city},
        {"to_code", d.to_code},
        {"date", d.date},
        {"routes", d.routes},
        {"total", d.total}
    };
}

inline void from_json(const json& j, Data& d) {
    d.from_city = j.value("from_city", "");
    d.from_code = j.value("from_code", "");
    d.to_city   = j.value("to_city", "");
    d.to_code   = j.value("to_code", "");
    d.date      = j.value("date", "");
    d.total     = j.value("total", 0);

    if (j.contains("routes") && j["routes"].is_array()) {
        d.routes = j["routes"].get<std::vector<Route>>();
    } else {
        d.routes.clear();
    }
}

inline void to_json(json& j, const CacheEntry& e) {
    j = json{
        {"key", e.key},
        {"data", e.data},
        {"timestamp", e.timestamp}
    };
}

inline void from_json(const json& j, CacheEntry& e) {
    e.key       = j.value("key", "");
    e.timestamp = j.value("timestamp", static_cast<int64_t>(0));

    if (j.contains("data")) {
        e.data = j["data"].get<Data>();
    } else {
        e.data = Data{};
    }
}