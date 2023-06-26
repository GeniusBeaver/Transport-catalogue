#include <iostream>
#include "transport_catalogue.h"
#include "json_reader.h"
#include "request_handler.h"
#include <sstream>
using namespace std;

int main() {
    gid::TransportCatalogue test;
    gid::reader::ReaderJson read(cin, test, cout);
    read.ReadCatalogue();
    //test.AddStop("lol", 1, 2);
    /*auto temp = test.GetStopId();
    cout << temp.size();
    
    for (const auto temp1 : temp) {
        cout << temp1.first->name;
    }*/
    /*auto temp = test.GetStopId();
    for (const auto& i : temp) {
        cout << i.first->name << " " << i.second << endl;
    }*/
}