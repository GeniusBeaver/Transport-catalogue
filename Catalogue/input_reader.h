#pragma once
#include "transport_catalogue.h"
#include <iostream>
#include <sstream>
namespace gid{
    namespace in{
std::vector<std::string_view> SplitIntoWords(std::string_view text);
void AddBus(std::string& line, gid::TransportCatalogue& guide);
void AddStop(std::string& line, gid::TransportCatalogue& guide);

void ReadCatalogue(std::istream& input, gid::TransportCatalogue& guide);
    }
}
