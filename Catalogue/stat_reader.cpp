#include "stat_reader.h"
using namespace std;
namespace gid{
namespace out{

void PtintBusResult(string& line, std::ostream& out, gid::TransportCatalogue& guide){
    line = line.substr(4);
    line = line.substr(line.find_first_not_of(' ',0));
    auto temp = line.find_last_not_of(' ');
    string name = line.substr(0, ++temp);
    
    if (guide.HasBus(name)){
        out << "Bus " << name << ": not found" << endl;
        return;
    } 
    
    auto result = guide.GetResultBus(name);
    out << "Bus " << name << ": " << (*result).stops_on_route << " stops on route, ";
    out << (*result).unique_stops << " unique stops, ";
    out << (*result).distance << " route length, ";
    out << (*result).curvature << " curvature" << endl;
}

void PrintStopResult(string& line, std::ostream& out, gid::TransportCatalogue& guide){
    line = line.substr(4);
    line = line.substr(line.find_first_not_of(' ',0));
    auto temp = line.find_last_not_of(' ');
    string name = line.substr(0, ++temp);
    if (guide.HasStop(name)){
        out << "Stop " << name << ": not found" << endl;
        return;
    }  
    
    auto result = guide.GetResultStop(name);
    out << "Stop " << name << ": ";
    if ((*(*result).buses).size() == 0) {
        out << "no buses" << endl;
        return;
    }
    
    out << "buses ";
    for (const auto& i : (*(*result).buses)){
        out << i << " ";
    }
    out << endl;
}

void Results(ostream& out, istream& input, gid::TransportCatalogue& guide){
    int count;
    input >> count;
    string line;
    
    for (int i = 0; i <= count; i++){
        getline(input, line);
        auto ch = line.find_first_not_of(' ',0);
        if (ch != line.npos){
            line = line.substr(ch);
            if (line[0] == 'B'){
                PtintBusResult(line, out, guide);
            } else if (line[0] == 'S'){
                PrintStopResult(line, out, guide);
            }
        }
    }
}
}    
}    