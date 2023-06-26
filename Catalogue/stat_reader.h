#pragma once
#include "transport_catalogue.h"
#include "input_reader.h"
#include <iostream>
#include "geo.h"

namespace gid{
namespace out{

void PtintBusResult(std::string& line, std::ostream& out, gid::TransportCatalogue&);
void PrintStopResult(std::string& line, std::ostream& out, gid::TransportCatalogue&);

void Results(std::ostream& out, std::istream& input, gid::TransportCatalogue& guide);
}    
}