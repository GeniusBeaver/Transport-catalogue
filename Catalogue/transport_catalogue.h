#pragma once
#include <string_view>
#include <deque>
#include <string>
#include <unordered_set>
#include <unordered_map>
#include <set>
#include <map>
#include <algorithm>
#include <mutex>
#include <vector>
#include <iostream>
#include "geo.h"

namespace gid{
struct pair_hash {
  template <class T1, class T2>
  std::size_t operator()(const std::pair<T1, T2>& pair) const {
    std::size_t h1 = std::hash<T1>{}(pair.first);
    std::size_t h2 = std::hash<T2>{}(pair.second);
    return hash_combine(h1, h2);
  }
   template <class T>
  static inline size_t hash_combine(std::size_t& seed, const T& val) {
    seed ^= std::hash<T>{}(val) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
    return seed;
  }
};

struct Stop{
    std::string_view name;
    double x = 0;
    double y = 0;
    
    Stop(std::string_view namet, double xt, double yt) : name(namet), x(xt), y(yt) {}
    Stop(std::string_view namet) : name(namet){}
};

struct Bus{
    std::string_view name;
    std::deque<Stop*> route;
    std::string_view type;
    
    Bus(std::string_view namet, std::string_view typet) : name(namet), type(typet) {}
};
    
struct ResultStop{
    std::string_view name;
    std::set<std::string_view>* buses;
    
    ResultStop(std::string_view namet, std::set<std::string_view>* busest) : name(namet), buses(busest){
    } 
};
    
struct ResultBus{
    std::string_view name;
    int stops_on_route;
    int unique_stops;
    int distance;
    double curvature;
    
    ResultBus(std::string_view namet, size_t stops, size_t unique, int dis, double cul) : name(namet), 
    stops_on_route(stops), 
    unique_stops(unique), 
    distance(dis), 
    curvature(cul){}
};

class TransportCatalogue{
public:
    TransportCatalogue() = default;
    
    std::unordered_map <std::string_view, Stop*>* GetStop();
    std::deque<Bus>* GetBuses();
    Bus* GetBus(std::string_view name);
    Stop* GetStop(std::string_view name);
    std::set<std::string_view>* GetStopToBus(const std::string& name);
    bool HasStop(std::string_view name);
    bool HasBus(std::string_view name);
    int GetDistanceBetweenStops(std::string_view first, std::string_view second) const;
    std::unordered_map<std::pair<std::string_view, std::string_view>, int, pair_hash>* GetDistancesBetweenStops();
    ResultStop* GetResultStop(const std::string& name);
    ResultBus* GetResultBus(const std::string& name);
    void BuildDistanceCaches();
    std::set<std::string> GetNonEmptyStop() const;
    geo::Coordinates GetCoords(const std::string& name) const;
    std::unordered_map<const Stop*, size_t> GetStopId() const;
    
    void AddBus(const std::string& name, const std::deque<std::string>& route, const std::string& type);
    void AddStop(const std::string& name, double x, double y);
    void AddComputedDistance(const std::string& name, const std::string& name_to, int dis);
    
    
private:
    size_t id_ = 0;
    std::deque<Bus> buses_;
    std::unordered_map <std::string_view, Stop*> stops_;
    std::deque<std::string> all_names_ = {">", "-"};
    std::unordered_map<const Stop*, size_t> stop_id_;
    std::unordered_map<std::string_view, std::set<std::string_view>> stop_to_bus_;
    std::unordered_map<std::pair<std::string_view, std::string_view>, int, pair_hash> real_distance_;
    std::unordered_map<std::string_view, ResultStop*> result_stop_;
    std::unordered_map<std::string_view, ResultBus*> result_bus_;
};
}    