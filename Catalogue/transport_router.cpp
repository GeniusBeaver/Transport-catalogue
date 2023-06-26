#include "transport_router.h"
using namespace std;
const int zero_distance = 0;
const double km = 1000.0;
const double hour = 60.0;

namespace gid {
void OptimalRoute::CreateGraph() {
    auto StopId = guide_.GetStopId();
    DistancePair id_to_distance;
    
    for (const Bus& bus : *(guide_.GetBuses())) {
        for (auto stop = bus.route.begin(); stop + 1 != bus.route.end(); ++stop) {
            int span_count = 0;
            int number = 0;
            
            for (auto stop2 = stop + 1; stop2 != bus.route.end(); ++stop2) { 
                ++span_count;
                auto prev_stop = stop + number;
                
                graph::VertexId id_from = StopId.at(*stop);
                graph::VertexId id_to = StopId.at(*prev_stop);
                graph::VertexId id_current = StopId.at(*stop2);
                distance_[{id_from, id_from}] = 0;
                
                int distance1 = guide_.GetDistanceBetweenStops((*prev_stop)->name, (*stop2)->name);
                distance_[{ id_from, id_current }] = distance_.at({ id_from,  id_to }) + distance1;
                
                RouteToStop edgep{&bus, span_count, distance_.at({ id_from,  id_to }) + distance1, 0.0, (*stop)->name};

                auto it_find = id_to_distance.find({ id_from,  id_current });
                if (it_find == id_to_distance.end()) {
                    id_to_distance[{ id_from, id_current }] = edgep;
                }
                else if (it_find->second.distance > edgep.distance) {
                    id_to_distance[{ id_from, id_current }] = edgep;
                }
                        
                if (bus.type== "-") {
                    int distance2 = guide_.GetDistanceBetweenStops((*stop2)->name, (*prev_stop)->name);
                    distance_[{ id_current, id_from }] = distance_.at({ id_to, id_from }) + distance2;
                    
                    RouteToStop temp_edge(edgep);
                    temp_edge.distance = distance_.at({ id_to, id_from }) + distance2;
                    temp_edge.stop_from = (*stop2)->name;
                    
                    auto it_find = id_to_distance.find({ id_current, id_from});
                    if (it_find == id_to_distance.end()) {
                        id_to_distance[{ id_current, id_from }] = temp_edge;
                    }
                    else if (it_find->second.distance > temp_edge.distance) {
                        id_to_distance[{ id_current, id_from}] = temp_edge;
                    }
                }
                
                ++number;
            }
        }
    }
    SaveResult(id_to_distance);
    router_ = new graph::Router<double>(graph_);
}
    
void OptimalRoute::SaveResult(const DistancePair& id_to_distance) {
    for (const auto& result : id_to_distance) {
        double route_time = ((double(result.second.distance) / km) / double(V_bus_)) * hour + wait_time_;
        graph::EdgeId id = graph_.AddEdge({ result.first.first, result.first.second, route_time });
        
        RouteToStop edge(result.second);
        edge.route_time = route_time;
        Id_to_props_.emplace(id, edge);
    }
}
    
ResultRoute OptimalRoute::GetRoute(const std::string& from, const std::string& to) {
    auto StopId = guide_.GetStopId();
    auto index_from = StopId.at(guide_.GetStop(from));
    auto index_to = StopId.at(guide_.GetStop(to));   
    auto chek = router_->BuildRoute(index_from, index_to);
    ResultRoute result;
    double total_time = 0.0;
    
    if (!chek)
        return result;
    
    for (const auto& stop_id : chek.value().edges) {
        result.items.push_back(RouteElement{false, string(Id_to_props_.at(stop_id).stop_from), ""s, double(wait_time_), 0});
        result.items.push_back(RouteElement{true, ""s, string(Id_to_props_.at(stop_id).bus->name), Id_to_props_.at(stop_id).route_time - wait_time_, Id_to_props_.at(stop_id).span_count});
        total_time += Id_to_props_.at(stop_id).route_time;
    }
    
    result.total_time = total_time;
    return result;
}
    
}