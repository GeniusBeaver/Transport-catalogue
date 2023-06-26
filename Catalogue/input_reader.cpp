#include "input_reader.h"
#include <string.h>
using namespace std;

namespace gid{
namespace in{
std::vector<std::string_view> SplitIntoWords(std::string_view text) {
    std::vector<std::string_view> result;
    const int64_t pos_end = text.npos;
    text.remove_prefix(std::min(text.find_first_not_of(" "), text.size()));
    while (!text.empty()) {
        int64_t space = text.find(' ');

        if (space == pos_end) {
            result.push_back(text.substr(0, text.size()));
        }
        else {
           result.push_back(text.substr(0, space));
         }
       text.remove_prefix(std::min(text.size(), text.find_first_not_of(' ', space)));
    }
    return result;
}
    
void AddBus(std::string& line, gid::TransportCatalogue& guide){
    deque<string> route;
    line = line.substr(4);
    line = line.substr(line.find_first_not_of(' ',0));
    auto temp = line.find(':');
    string type = ">";
    if (line.find("-") != line.npos) type = "-";
    string name = line.substr(0, temp);
    line = line.substr(++temp);
    if (type == ">"){
        stringstream str(line);
        string result;
        while (getline(str, result, '>')){
            result = result.substr(result.find_first_not_of(' '));
            if (result[result.size() - 1] == ' '){
                result.resize(result.size() - 1);
            }
            route.push_back(result);
         }
    }else {
        stringstream str(line);
        string result;
        while (getline(str, result, '-')){
            result = result.substr(result.find_first_not_of(' '));
            if (result[result.size() - 1] == ' '){
                result.resize(result.size() - 1);
            }
            route.push_back(result);
        }
    }
                
    guide.AddBus(name, route, type);
}
    
void AddStop(std::string& line, gid::TransportCatalogue& guide){
    line = line.substr(5);
    line = line.substr(line.find_first_not_of(' ',0));
    auto temp = line.find(':');
    string name = line.substr(0, line.find(':'));
    line = line.substr(++temp);
    auto vec = SplitIntoWords(line);
    guide.AddStop(name, stod(string(vec[0])), stod(string(vec[1])));
                
    line = line.substr(line.find(' ') + 1);
    line = line.substr(line.find(' ') + 1);
                
    stringstream str(line);
    string result;
    while (getline(str, result, ',')){
        result = result.substr(result.find_first_not_of(' '));
        int distance = stoi(result.substr(0, result.find('m')));
        result = result.substr(result.find("to") + 3);
        guide.AddComputedDistance(name, result, distance);
    }
}

void ReadCatalogue(istream& input, gid::TransportCatalogue& guide){
    int count;
    input >> count;
    string line;
    
    for (int i = 0; i <= count; i++){
        getline(input, line);
        auto ch = line.find_first_not_of(' ',0);
        if (ch != line.npos){
            line = line.substr(ch);
            
            if (line[0] == 'B') {
                AddBus(line, guide);
                
            } else if (line[0] == 'S') {
                AddStop(line, guide);
            }   
        }           
    }
    guide.BuildDistanceCaches();   
}
}
}
