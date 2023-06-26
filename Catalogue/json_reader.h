#pragma once
#include "json.h"
#include "map_renderer.h"
#include "transport_catalogue.h"
#include <iostream>
#include "json_builder.h"
#include <string.h>
#include <sstream>
#include "transport_router.h"

namespace gid{
namespace reader{
    
class ReaderJson {
private:
    gid::TransportCatalogue& guide_;
    std::istream& input_;
    std::ostream& out_;
    int wait_time_;
    double V_bus_;
    
    void AddBus(const json::Node& json);
    void AddStop(const json::Node& json);
    json::Node GetBusDict(const std::string& name, int id);
    json::Node GetStopDict(const std::string& name, int id);
    json::Node DrawMap(int id, const json::Node& doc);
    json::Node GetRoute(int id, const std::string& from, const std::string& to, OptimalRoute& route);
    void ExecuteStatRequests(const json::Dict& json);
    
public:
    ReaderJson(std::istream& input, gid::TransportCatalogue& guide, std::ostream& out) : guide_(guide),
        input_(input),
        out_(out){}
    
    void ReadCatalogue();
    
};
    
}
}