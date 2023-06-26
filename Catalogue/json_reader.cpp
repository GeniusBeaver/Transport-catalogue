#include "json_reader.h"
using namespace std;

namespace gid{
namespace reader{
    
void ReaderJson::AddBus(const json::Node& json){
    using namespace std::literals;
    deque<string> route;
    const auto& json_array = (json.AsDict()).at("stops"s);
    
    for (const auto& stop : json_array.AsArray()) {
        route.push_back(stop.AsString());
    }

    string type = "-";

    if (json.AsDict().at("is_roundtrip"s).AsBool()) type = ">";
    
    guide_.AddBus(json.AsDict().at("name"s).AsString(), route, type);
}
    
void ReaderJson::AddStop(const json::Node& json){
    using namespace std::literals;
    const auto& road = json.AsDict().at("road_distances"s);
    string name = json.AsDict().at("name"s).AsString();
    
    guide_.AddStop(name, json.AsDict().at("latitude"s).AsDouble(), json.AsDict().at("longitude"s).AsDouble());
    
    for (const auto& i : road.AsDict()) {
        string nameto = i.first;
        guide_.AddComputedDistance(name, nameto, i.second.AsInt());
    }
}
    
json::Node ReaderJson::GetBusDict(const string& name, int id) {
    using namespace std::literals;
    json::Builder result;
    result.StartDict().Key("request_id"s).Value(id);
    if (guide_.HasBus(name)) {
        result.Key("error_message"s).Value("not found"s);
        return result.EndDict().Build();
    } 
    
    auto result_data = guide_.GetResultBus(name);
    result.Key("curvature"s).Value((*result_data).curvature);
    result.Key("route_length"s).Value((*result_data).distance);
    result.Key("stop_count"s).Value((*result_data).stops_on_route);
    result.Key("unique_stop_count"s).Value((*result_data).unique_stops);
    return result.EndDict().Build();
}

json::Node ReaderJson::GetStopDict(const string& name, int id) {
    json::Builder result;
    result.StartDict().Key("request_id"s).Value(id);
    
    if (guide_.HasStop(name)){
        result.Key("error_message"s).Value("not found"s).EndDict();
        return result.Build();
    }  
    
    auto result_data = guide_.GetResultStop(name);
    result.Key("buses"s).StartArray();
    for (const auto& i : (*(*result_data).buses)){
        result.Value(string(i));
    }
    
    auto temp = result.EndArray().EndDict().Build();
    return temp;
}
    
json::Node ReaderJson::DrawMap(int id, const json::Node& doc) {
    ostringstream line;
    gid::draw::MapReader reader(guide_, doc.AsDict());
    reader.Draw(line);
    string result = line.str();
    auto temp = json::Builder{}.StartDict().Key("request_id"s).Value(id).Key("map"s).Value(result).EndDict().Build();
    return temp; 
}
    
json::Node ReaderJson::GetRoute(int id, const std::string& from, const std::string& to, OptimalRoute& route) {
    json::Builder json;
    json.StartDict().Key("request_id"s).Value(id);
    if (from == to) {
        json.Key("items"s).StartArray().EndArray();
        return json.Key("total_time"s).Value(0).EndDict().Build();
    }
    
    auto result = route.GetRoute(from, to);
    if (result.items.empty()) {
        json.Key("error_message"s).Value("not found"s);
        return json.EndDict().Build();
    }
    
    json.Key("total_time"s).Value(result.total_time);
    json.Key("items"s).StartArray();
    for (const auto& temp : result.items) {
        json.StartDict();
        if (temp.type) {
            json.Key("type"s).Value("Bus"s);
            json.Key("bus"s).Value(temp.bus_name);
            json.Key("span_count"s).Value(temp.span_count);
            json.Key("time"s).Value(temp.time);
        } else {
            json.Key("type"s).Value("Wait"s);
            json.Key("stop_name"s).Value(temp.stop_name);
            json.Key("time"s).Value(temp.time);
        }
        json.EndDict();
    }
    
    return json.EndArray().EndDict().Build();
}

void ReaderJson::ExecuteStatRequests(const json::Dict& json) {
    using namespace std::literals;
    
    const auto& stats = json.at("stat_requests"s).AsArray();
    json::Array result;
    
    OptimalRoute route(guide_, wait_time_, V_bus_);
    route.CreateGraph();
    
    for (const auto& stat : stats) {
        if (stat.AsDict().at("type"s) == "Bus"s) {
            result.push_back(GetBusDict(stat.AsDict().at("name"s).AsString(), stat.AsDict().at("id"s).AsInt()));
        } else if (stat.AsDict().at("type"s) == "Stop"s) {
            result.push_back(GetStopDict(stat.AsDict().at("name"s).AsString(), stat.AsDict().at("id"s).AsInt()));
        } else if (stat.AsDict().at("type"s).AsString() == "Map"s) {
            result.push_back(DrawMap(stat.AsDict().at("id"s).AsInt(), json.at("render_settings"s)));
        } else if (stat.AsDict().at("type"s).AsString() == "Route"s) {
            result.push_back(GetRoute(stat.AsDict().at("id"s).AsInt(), stat.AsDict().at("from"s).AsString(), stat.AsDict().at("to"s).AsString(), route));
        }
    }
    
    json::Print(json::Document{result}, out_);
}

void ReaderJson::ReadCatalogue(){
    using namespace std::literals;
    auto doc = json::Load(input_);
    
    V_bus_ = doc.GetRoot().AsDict().at("routing_settings"s).AsDict().at("bus_velocity"s).AsDouble();
    wait_time_ = doc.GetRoot().AsDict().at("routing_settings"s).AsDict().at("bus_wait_time"s).AsInt();
    
    const auto& json_array = ((doc.GetRoot()).AsDict()).at("base_requests"s);
    
    for (const auto& file : json_array.AsArray()) {
        const auto& json_obj = file.AsDict();
        if (json_obj.at("type"s) == "Stop"s) {
            AddStop(json_obj);
            continue;
        } else if (json_obj.at("type"s) == "Bus"s) {
            AddBus(json_obj);
            continue;
        }
    }
    
    guide_.BuildDistanceCaches();
    ExecuteStatRequests(doc.GetRoot().AsDict());
}
}
}