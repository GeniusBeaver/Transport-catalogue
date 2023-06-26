#pragma once
#include "json.h"
#include "transport_catalogue.h"
#include "router.h"
#include <algorithm>
#include <queue>
#include <iostream>

namespace gid {
  
struct RouteElement {
    bool type;
    std::string stop_name;
    std::string bus_name;
    double time = 0.0;
    int span_count = 0;
};

struct ResultRoute {
    double total_time = 0.0;
    std::vector<RouteElement> items;
};

struct RouteToStop {
    const gid::Bus* bus;
    int span_count = 0;
    int distance = 0;
    double route_time = 0.0;
    std::string_view stop_from;
};   
    
using DistancePair = std::unordered_map<std::pair<graph::VertexId, graph::VertexId>, RouteToStop, gid::pair_hash>;
    
class OptimalRoute {
public:
    OptimalRoute(gid::TransportCatalogue& guide, int wait_time, double V_bus) :
        guide_(guide),
        wait_time_(wait_time),
        V_bus_(V_bus),
        graph_(guide.GetStopId().size()) {}
        
    void CreateGraph();
    ResultRoute GetRoute(const std::string& from, const std::string& to);
    
private:
    gid::TransportCatalogue& guide_;   
    int wait_time_;
    double V_bus_;
    graph::Router<double>* router_;
    graph::DirectedWeightedGraph<double> graph_;
    std::unordered_map<std::pair<graph::VertexId, graph::VertexId>, int, gid::pair_hash> distance_;
    std::unordered_map<graph::EdgeId, RouteToStop> Id_to_props_;
    
    void SaveResult(const DistancePair& tmp_pair_id_to_distance);
};
        
}
