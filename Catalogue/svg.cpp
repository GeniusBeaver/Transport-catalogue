#include "svg.h"

namespace svg {

using namespace std::literals;
    
std::ostream& operator<<(std::ostream& os, const StrokeLineCap& cap) {
    switch (cap) {
        case StrokeLineCap::BUTT: os << "butt"; break;
        case StrokeLineCap::ROUND: os << "round"; break;
        case StrokeLineCap::SQUARE: os << "square"; break;
    }
    return os;
}


std::ostream& operator<<(std::ostream& os, const StrokeLineJoin& join) {
    switch (join) {
        case StrokeLineJoin::ARCS: os << "arcs"; break;
        case StrokeLineJoin::BEVEL: os << "bevel"; break;
        case StrokeLineJoin::MITER: os << "miter"; break;
        case StrokeLineJoin::MITER_CLIP: os << "miter-clip"; break;
        case StrokeLineJoin::ROUND: os << "round"; break;
    }
    return os;
}
    
std::ostream& operator<<(std::ostream& out, const Rgb& line) {
    out << "rgb(" << static_cast<unsigned int>(line.red) << "," << static_cast<unsigned int>(line.green) << "," << static_cast<unsigned int>(line.blue) << ")";
    return out;
}
    
std::ostream& operator<<(std::ostream& out, const Rgba& line) {
    out << "rgba(" << static_cast<unsigned int>(line.red) << "," << static_cast<unsigned int>(line.green) << "," << static_cast<unsigned int>(line.blue) << "," << (line.opacity) << ")";
    return out;
}    
    
void Object::Render(const RenderContext& context) const {
    context.RenderIndent();

    // Делегируем вывод тега своим подклассам
    RenderObject(context);

    context.out << std::endl;
}

// ---------- Circle ------------------

Circle& Circle::SetCenter(Point center)  {
    center_ = center;
    return *this;
}

Circle& Circle::SetRadius(double radius)  {
    radius_ = radius;
    return *this;
}

void Circle::RenderObject(const RenderContext& context) const {
    auto& out = context.out;
    out << "<circle cx=\""sv << center_.x << "\" cy=\""sv << center_.y << "\" "sv;
    out << "r=\""sv << radius_ << "\""sv;
    RenderAttrs(context.out);
    out << "/>"sv;
}
    
Polyline& Polyline::AddPoint(Point point) {
    points_.push_back(point);
    return *this;
}
    
void Polyline::RenderObject(const RenderContext& context) const {
    auto& out = context.out;
    out << "<polyline points=\""sv;
    for (auto i = points_.begin(); i < points_.end(); i++){
        out << i->x << ","sv << i->y;
        if (i != points_.end() - 1) out << " ";
    }
    out << "\"";
    RenderAttrs(context.out);
    out <<"/>";
}
    
Text& Text::SetPosition(Point pos){
    point_ = pos;
    return *this;
}

Text& Text::SetOffset(Point offset){
    dpoint_ = offset;
    return *this;
}

Text& Text::SetFontSize(uint32_t size){
    font_size_ = size;
    return *this;
}

Text& Text::SetFontFamily(std::string font_family){
    font_family_ = font_family;
    return *this;
}

Text& Text::SetFontWeight(std::string font_weight){
    font_weight_ = font_weight;
    return *this;
}

Text& Text::SetData(std::string data){
    data_ = data;
    return *this;
}
    
void Text::RenderObject(const RenderContext& context) const {
    auto& out = context.out;
    
    out << "<text";
    RenderAttrs(context.out);
    out << " x=\"" << point_.x << "\" y=\"" << point_.y << "\"";
    
    out << " dx=\"" << dpoint_.x << "\" dy=\"" << dpoint_.y << "\"";
	
	out << " font-size=\"" << font_size_ << "\"";
    if (!font_weight_.empty()) {
		out << " font-weight=\"" << font_weight_ << "\"";
	}
    if (!font_family_.empty()) {
		out << " font-family=\"" << font_family_ << "\"";
	}
	
    out << ">" << data_ << "</text>";
    
}
    
void Document::Render(std::ostream& out) const{
        out << R"(<?xml version="1.0" encoding="UTF-8" ?>)" << std::endl;
		out << R"(<svg xmlns="http://www.w3.org/2000/svg" version="1.1">)" << std::endl;
        RenderContext context(out, 2);
        for (const auto& i : object_) {
            i->Render(context.Indented());
        }
        out << "</svg>";
    }

}  // namespace svg