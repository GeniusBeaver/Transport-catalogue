#include "map_renderer.h"
using namespace std;

namespace gid {
namespace draw {
    
svg::Color GetColor(const json::Node& node) {
    svg::Color color;
    
    if (node.IsString()) {
            color = node.AsString();
            return color;
    } else {
        if (node.AsArray().size() == 3) {
            color = svg::Rgb(node.AsArray()[0].AsInt(), node.AsArray()[1].AsInt(), node.AsArray()[2].AsInt()); 
            return color;
        } else {
            color = svg::Rgba(node.AsArray()[0].AsInt(), node.AsArray()[1].AsInt(), node.AsArray()[2].AsInt(), node.AsArray()[3].AsDouble());
            return color;
        }
    }
}
    
SphereProjector GetProj(const json::Dict& data, gid::TransportCatalogue& guide) {
    const double WIDTH = data.at("width"s).AsDouble();
    const double HEIGHT = data.at("height"s).AsDouble();
    const double PADDING = data.at("padding"s).AsDouble();
    vector<geo::Coordinates> geo_coords;
    
    for (const auto& Bus : (*(guide.GetBuses()))) {
        for (const auto& stop : Bus.route) {
            geo_coords.push_back(geo::Coordinates{stop->x, stop->y});
        }
    }
    
    return SphereProjector {
        geo_coords.begin(), geo_coords.end(), WIDTH, HEIGHT, PADDING
    };
}
    
void MapReader::LineForRound(const gid::Bus& Bus, svg::Polyline& line) {
    for (const auto& geo_coord : Bus.route) {
        const svg::Point screen_coord = proj_(geo::Coordinates{geo_coord->x, geo_coord->y});
        line.AddPoint(screen_coord);
    }
                line.SetFillColor("none"s).SetStrokeWidth(data_.at("line_width"s).AsDouble()).SetStrokeLineCap(svg::StrokeLineCap::ROUND).SetStrokeLineJoin(svg::StrokeLineJoin::ROUND);
}
    
void MapReader::LineForLine(const gid::Bus& Bus, svg::Polyline& line) {
    for (const auto& geo_coord : Bus.route) {
        const svg::Point screen_coord = proj_(geo::Coordinates{geo_coord->x, geo_coord->y});
        line.AddPoint(screen_coord);
    }
    
    for (int i = Bus.route.size() - 2; i != -1; i--) {
        const svg::Point screen_coord = proj_(geo::Coordinates{Bus.route[i]->x, Bus.route[i]->y});
        line.AddPoint(screen_coord);
    }
            line.SetFillColor("none"s).SetStrokeWidth(data_.at("line_width"s).AsDouble()).SetStrokeLineCap(svg::StrokeLineCap::ROUND).SetStrokeLineJoin(svg::StrokeLineJoin::ROUND);
}
    
void MapReader::DrawPolyline(svg::Document& document) {
    using namespace std::literals;
    
    long unsigned int count = 0;
    
    auto sort_buses = *(guide_.GetBuses());
    std::sort(sort_buses.begin(), sort_buses.end(), [](const auto& bus1, const auto& bus2) {
        return (bus1).name < (bus2).name;
    });
    
    for (const auto& Bus : sort_buses) {
        svg::Polyline map;
        if (Bus.route.size() == 0) continue;
        if (Bus.type == ">") {
            LineForRound(Bus, map);
        } else {
            LineForLine(Bus, map);
        }
        
        auto& node = data_.at("color_palette"s).AsArray()[count];
        map.SetStrokeColor(GetColor(node));
        
        if (++count == data_.at("color_palette"s).AsArray().size()) count = 0;
        
        document.AddPtr(std::move(std::make_unique<svg::Polyline>(map)));
    }
}
    
void MapReader::DrawNameBus(svg::Document& document) {
    using namespace std::literals;
    
    long unsigned int count = 0;
    const double dx = data_.at("bus_label_offset"s).AsArray()[0].AsDouble();
    const double dy = data_.at("bus_label_offset"s).AsArray()[1].AsDouble();
    const int font_size = data_.at("bus_label_font_size"s).AsInt();
    const auto line_join = svg::StrokeLineJoin::ROUND;
    const auto line_cap = svg::StrokeLineCap::ROUND;
    const string font_family = "Verdana";
    const string font_weight = "bold";
    const svg::Color fill_sub = GetColor(data_.at("underlayer_color"s));
    const double width = data_.at("underlayer_width"s).AsDouble();
    
    auto sort_buses = *(guide_.GetBuses());
    std::sort(sort_buses.begin(), sort_buses.end(), [](const auto& bus1, const auto& bus2) {
        return (bus1).name < (bus2).name;
    });
    
    for (const auto& Bus : sort_buses) {
        if (Bus.route.size() == 0) continue;
        string name(Bus.name);
        
        if (Bus.type == ">" || (Bus.type == "-" && Bus.route[0] == Bus.route.back())) {
            svg::Text subsrate;
            svg::Text text;
            const svg::Point point = proj_(geo::Coordinates{Bus.route[0]->x, Bus.route[0]->y});
            
            subsrate.SetPosition(point).SetOffset(svg::Point{dx, dy}).SetFontSize(font_size).SetFontFamily(font_family).SetFontWeight(font_weight).SetData(name);
            subsrate.SetFillColor(fill_sub).SetStrokeColor(fill_sub).SetStrokeWidth(width).SetStrokeLineCap(line_cap);
            subsrate.SetStrokeLineJoin(line_join);
            
            text.SetPosition(point).SetOffset(svg::Point{dx, dy}).SetFontSize(font_size).SetFontFamily(font_family).SetFontWeight(font_weight).SetData(name);
            text.SetFillColor(GetColor(data_.at("color_palette"s).AsArray()[count]));
            
            document.AddPtr(std::move(std::make_unique<svg::Text>(subsrate)));
            document.AddPtr(std::move(std::make_unique<svg::Text>(text)));
        } else {
            for (int i = 0; i != 2; i++) {
                svg::Text subsrate;
                svg::Text text;
                
                subsrate.SetFontSize(font_size).SetFontFamily(font_family).SetFontWeight(font_weight).SetData(name);
                subsrate.SetFillColor(fill_sub).SetStrokeColor(fill_sub).SetStrokeWidth(width).SetStrokeLineCap(line_cap);
                subsrate.SetStrokeLineJoin(line_join).SetOffset(svg::Point{dx,dy});
                
                text.SetFontSize(font_size).SetFontFamily(font_family).SetFontWeight(font_weight).SetData(name);
                text.SetFillColor(GetColor(data_.at("color_palette"s).AsArray()[count])).SetOffset(svg::Point{dx,dy});
                
                if (i == 1) {
                    const svg::Point point = proj_(geo::Coordinates{Bus.route.back()->x, Bus.route.back()->y});
                    subsrate.SetPosition(point).SetOffset(svg::Point{dx,dy});
                    text.SetPosition(point).SetOffset(svg::Point{dx,dy});
                    
                    document.AddPtr(std::move(std::make_unique<svg::Text>(subsrate)));
                    document.AddPtr(std::move(std::make_unique<svg::Text>(text)));
                } else {
                    const svg::Point point = proj_(geo::Coordinates{Bus.route[0]->x, Bus.route[0]->y});
                    
                    subsrate.SetPosition(point);
                    text.SetPosition(point);
                    
                    document.AddPtr(std::move(std::make_unique<svg::Text>(subsrate)));
                    document.AddPtr(std::move(std::make_unique<svg::Text>(text)));
                }
            } 
        }

        if (++count == data_.at("color_palette"s).AsArray().size()) count = 0;
    }
}
    
void MapReader::DrawCircle(svg::Document& document) {
    const double rad = data_.at("stop_radius"s).AsDouble();
    auto stops = guide_.GetNonEmptyStop();
    
    for (const auto& stop : stops) {
        const svg::Point point = proj_(guide_.GetCoords(stop));
        svg::Circle circle;
        circle.SetRadius(rad).SetCenter(point).SetFillColor("white");
        document.AddPtr(std::move(std::make_unique<svg::Circle>(circle)));
    }
}
    
void MapReader::DrawNameStop(svg::Document& document) {
    using namespace std::literals;
    
    const double dx = data_.at("stop_label_offset"s).AsArray()[0].AsDouble();
    const double dy = data_.at("stop_label_offset"s).AsArray()[1].AsDouble();
    const int font_size = data_.at("stop_label_font_size"s).AsInt();
    const auto line_join = svg::StrokeLineJoin::ROUND;
    const auto line_cap = svg::StrokeLineCap::ROUND;
    const string font_family = "Verdana";
    const svg::Color fill_sub = GetColor(data_.at("underlayer_color"s));
    const double width = data_.at("underlayer_width"s).AsDouble();
    
    auto stops = guide_.GetNonEmptyStop();
    for (const auto& stop : stops) {
        const svg::Point point = proj_(guide_.GetCoords(stop));
        svg::Text subsrate;
        svg::Text text;
        
        subsrate.SetPosition(point).SetOffset(svg::Point{dx,dy}).SetFontFamily(font_family).SetFontSize(font_size).SetData(stop);
        subsrate.SetFillColor(fill_sub).SetStrokeColor(fill_sub).SetStrokeWidth(width).SetStrokeLineCap(line_cap);
            subsrate.SetStrokeLineJoin(line_join);
        text.SetPosition(point).SetOffset(svg::Point{dx,dy}).SetFontFamily(font_family).SetFontSize(font_size).SetData(stop).SetFillColor("black");
        
        document.AddPtr(std::move(std::make_unique<svg::Text>(subsrate)));
        document.AddPtr(std::move(std::make_unique<svg::Text>(text)));
    }
}

void MapReader::Draw(ostream& out) {
    using namespace std::literals;
    
    svg::Document document;
    
    DrawPolyline(document);
    DrawNameBus(document);
    DrawCircle(document);
    DrawNameStop(document);

    document.Render(out);
}
    
}
}