#pragma once
#include <nlohmann/json.hpp>
using json = nlohmann::json;
#include "model.h"
#include "cache.h"

void to_json(json& j, const Segment& s) {
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

void from_json(const json& j, Segment& s) {
    j.at("transport_type").get_to(s.transport_type);
    j.at("number").get_to(s.number);
    j.at("title").get_to(s.title);
    j.at("carrier").get_to(s.carrier);
    j.at("vehicle").get_to(s.vehicle);
    j.at("from_station").get_to(s.from_station);
    j.at("to_station").get_to(s.to_station);
    j.at("departure").get_to(s.departure);
    j.at("arrival").get_to(s.arrival);
    j.at("duration").get_to(s.duration);
}

void to_json(json& j, const Route& r) {
    j = json{
        {"has_transfers", r.has_transfers},
        {"segments", r.segments},
        {"transfer_city", r.transfer_city},
        {"transfer_duration", r.transfer_duration},
        {"departure", r.departure},
        {"arrival", r.arrival}
    };
}

void from_json(const json& j, Route& r) {
    j.at("has_transfers").get_to(r.has_transfers);
    j.at("segments").get_to(r.segments);
    j.at("transfer_city").get_to(r.transfer_city);
    j.at("transfer_duration").get_to(r.transfer_duration);
    j.at("departure").get_to(r.departure);
    j.at("arrival").get_to(r.arrival);
}

void to_json(json& j, const Data& d) {
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

void from_json(const json& j, Data& d) {
    j.at("from_city").get_to(d.from_city);
    j.at("from_code").get_to(d.from_code);
    j.at("to_city").get_to(d.to_city);
    j.at("to_code").get_to(d.to_code);
    j.at("date").get_to(d.date);
    j.at("routes").get_to(d.routes);
    j.at("total").get_to(d.total);
}

void to_json(json& j, const CacheEntry& e) {
    j = json{
        {"key", e.key},
        {"data", e.data},
        {"timestamp", e.timestamp}
    };
}

void from_json(const json& j, CacheEntry& e) {
    j.at("key").get_to(e.key);
    j.at("data").get_to(e.data);
    j.at("timestamp").get_to(e.timestamp);
}