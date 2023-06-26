#include "transport_catalogue.h"
using namespace std;
namespace gid{
std::unordered_map <std::string_view, Stop*>* TransportCatalogue::GetStop() {
    return &stops_;
}

std::deque<Bus>* TransportCatalogue::GetBuses() {
    return &buses_;
}

Bus* TransportCatalogue::GetBus(std::string_view name) {
    auto temp = find_if(buses_.begin(), buses_.end(), [&](const auto& i){return i.name == name;});
    if (temp == buses_.end()) return new Bus("not found", ">");
    return &(*temp);
}
    
Stop* TransportCatalogue::GetStop(std::string_view name) {
    return stops_.at(name);
}

std::set<std::string_view>* TransportCatalogue::GetStopToBus(const string& name) {
    return &(stop_to_bus_[name]);
}

bool TransportCatalogue::HasStop(std::string_view name) {
    return !result_stop_.count(name);
}
    
bool TransportCatalogue::HasBus(std::string_view name) {
    return !result_bus_.count(name);
}

int TransportCatalogue::GetDistanceBetweenStops(std::string_view first, std::string_view second) const{
    if (real_distance_.count(make_pair(first, second))){
        return real_distance_.at(make_pair(first, second));
    } else {
        return real_distance_.at(make_pair(second, first));
    }
}

std::unordered_map<std::pair<std::string_view, std::string_view>, int, pair_hash>* TransportCatalogue::GetDistancesBetweenStops(){
    return &real_distance_;
}
    
ResultStop* TransportCatalogue::GetResultStop(const std::string& name){
    return result_stop_.at(name);
}
    
ResultBus* TransportCatalogue::GetResultBus(const std::string& name){
    return result_bus_.at(name);
}
    
void TransportCatalogue::BuildDistanceCaches(){
    //Stops result
    for (const auto& pair_stop : stops_){
        result_stop_[pair_stop.first] = new ResultStop(pair_stop.first, GetStopToBus(string(pair_stop.first)));
    }
    
    //Buses retult
    for (const auto& bus : buses_){
        unordered_set<string_view> set;
        for (const auto& i : bus.route) set.insert((*i).name);
        
        if (bus.type == "-"){
            double route_length = 0;
            int distance = 0;
            
            for (size_t number = 0; number < bus.route.size() - 1; number++){
                geo::Coordinates from{(*bus.route[number]).x, (*bus.route[number]).y};
                geo::Coordinates to{(*bus.route[number + 1]).x, (*bus.route[number + 1]).y};
                route_length += ComputeDistance(from, to);
                distance += GetDistanceBetweenStops((*bus.route[number]).name, (*bus.route[number + 1]).name);
                distance += GetDistanceBetweenStops((*bus.route[number + 1]).name, (*bus.route[number]).name);
            }
            
            route_length *= 2;
            double geo_dis = distance / route_length;
            result_bus_[bus.name] = new ResultBus(bus.name, bus.route.size() * 2 - 1, set.size(), distance, geo_dis);
            
        } else if (bus.type == ">"){
            double route_length = 0;
            int distance = 0;
            
            for (size_t number = 0; number < bus.route.size() - 1; number++){
                geo::Coordinates from{(*bus.route[number]).x, (*bus.route[number]).y};
                geo::Coordinates to{(*bus.route[number + 1]).x, (*bus.route[number + 1]).y};
                route_length += ComputeDistance(from, to);
                distance += GetDistanceBetweenStops((*bus.route[number]).name, (*bus.route[number + 1]).name);
            }
            
            double geo_dis = distance / route_length;
            result_bus_[bus.name] = new ResultBus(bus.name, bus.route.size(), set.size(), distance, geo_dis);
        }
    }
}

void TransportCatalogue::AddStop(const string& name, double x, double y){
    if (stops_.find(name) != stops_.end()) {
        (*(stops_[name])).x = x;
        (*(stops_[name])).y = y;
    } else {
        all_names_.push_back(name);
        stops_[all_names_.back()] = new Stop(all_names_.back(), x, y);
        stop_to_bus_[all_names_.back()];
        stop_id_[stops_[all_names_.back()]] = id_++;
    }
    
}

void TransportCatalogue::AddComputedDistance(const std::string& name, const std::string& name_to, int dis){
    auto namet = find(all_names_.begin(), all_names_.end(), name);
    
    if (namet == all_names_.end()) {
        AddStop(name, 0, 0);
        namet = find(all_names_.begin(), all_names_.end(), name);
    }
    
    auto nameto = find(all_names_.begin(), all_names_.end(), name_to);
    if (nameto == all_names_.end()){
        all_names_.push_back(name_to);
        real_distance_[make_pair(string_view(*namet), string_view(all_names_.back()))] = dis;
    } else {
        real_distance_[make_pair(string_view(*namet), string_view(*nameto))] = dis;
    }
}

void TransportCatalogue::AddBus(const string& name, const deque<string>& route, const std::string& type){
    all_names_.push_back(name);
    Bus result(all_names_.back(), all_names_[0]);
    if (type == "-")
        result.type = all_names_[1];
    
    for (const auto& stop : route){
        if (stops_.find(stop) != stops_.end()){
            result.route.push_back(stops_[stop]);
            stop_to_bus_[(*(stops_.find(stop))).first].insert(result.name);
        } else{
            all_names_.push_back(stop);
            stops_[all_names_.back()] = new Stop(all_names_.back());
            result.route.push_back(stops_[stop]);
            stop_to_bus_[(*(stops_.find(stop))).first].insert(result.name);
            stop_id_[stops_[all_names_.back()]] = id_++;
        }
    }
    buses_.push_back(move(result));
}
    
std::set<std::string> TransportCatalogue::GetNonEmptyStop() const {
    set<string> result;
    
    for (const auto& i : stop_to_bus_) {
        if (!i.second.empty()) result.insert(string(i.first));
    }
    
    return result;
}
    
geo::Coordinates TransportCatalogue::GetCoords(const std::string& name) const {
    return geo::Coordinates{stops_.at(name)->x, stops_.at(name)->y};
}
    
std::unordered_map<const Stop*, size_t> TransportCatalogue::GetStopId() const {
    return stop_id_;
}
    
}