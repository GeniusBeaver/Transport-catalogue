#include "json_builder.h"
using namespace std::literals;

namespace json {
    
ArrayItem KeyItem::StartArray() {
    return builder_.StartArray();
}    
    
DictItem KeyItem::StartDict() {
    return builder_.StartDict();
}  
    
DictItem KeyItem::Value(Node::Value value) {
    return DictItem(builder_.Value(value));
}
    
ArrayItem ArrayItem::StartArray() {
    return ArrayItem(builder_.StartArray());
}
    
DictItem ArrayItem::StartDict() {
    return builder_.StartDict();
}
    
ArrayItem ArrayItem::Value(Node::Value value) {
    return ArrayItem(builder_.Value(value));
}
    
Builder& ArrayItem::EndArray() {
    return builder_.EndArray();
}
    
KeyItem DictItem::Key(std::string str) {
    return builder_.Key(str);
}
    
Builder& DictItem::EndDict() {
    return builder_.EndDict();
}
    
DictItem Builder::StartDict() {
    if (close_) 
        throw std::logic_error("Node already has value"s);
    
   if (!stack.empty() && stack.back()->IsArray()) {
       stack.back()->AsArray().emplace_back(Dict());
       stack.push_back(&stack.back()->AsArray().back());
       return DictItem(*this);
    } else if (!stack.empty() && stack.back()->IsDict()) {
        if (!Dict_key.second) {
            throw std::logic_error("Key value not set"s);
        } else {
            stack.back()->AsDict().at(Dict_key.first).GetValue() = Dict();
            stack.push_back(&stack.back()->AsDict().at(Dict_key.first));
            Dict_key.second = false;
            return DictItem(*this);
        }
    } else if (stack.empty() && root_.IsNull()) {
       root_ = Dict();
       stack.push_back(&root_);
    }
    
    return DictItem(*this);
}  
    
KeyItem Builder::Key(std::string key) {
    if (stack.empty() || !stack.back()->IsDict()) 
        throw std::logic_error("Node is't a Dict"s);
    
    if (Dict_key.second) 
        throw std::logic_error("Key already has value"s);
    
    Dict_key.first = key;
    Dict_key.second = true;
    stack.back()->AsDict()[key];
    return *this;
}
    
Builder& Builder::EndDict() {
    if (!stack.empty() && stack.back()->IsDict()) {
        stack.pop_back();
        return *this;
    } else {
        throw std::logic_error("Don't start Dict"s);
    }
}
    
ArrayItem Builder::StartArray() {
    if (close_) 
        throw std::logic_error("Node already has value"s);
    
    if (!stack.empty() && stack.back()->IsArray()) {
        stack.back()->AsArray().emplace_back(Array());
        stack.push_back(&stack.back()->AsArray().back());
    } else if (!stack.empty() && stack.back()->IsDict()) {
        if (!Dict_key.second) {
                throw std::logic_error("Key value not set"s);
        } else {
            stack.back()->AsDict().at(Dict_key.first).GetValue() = Array();
            stack.push_back(&stack.back()->AsDict().at(Dict_key.first));
            Dict_key.second = false;
            return ArrayItem(*this);
        }
    } else if (stack.empty() && root_.IsNull()) {
        root_ = Array();
        stack.push_back(&root_);
    }
    return ArrayItem(*this);
}
    
Builder& Builder::EndArray() {
    if (!stack.empty() && stack.back()->IsArray()) {
        stack.pop_back();
        return *this;
    } else {
        throw std::logic_error("Don't start Array"s);
    }
}
    
Builder& Builder::Value(json::Node::Value value) {
    using namespace std::literals;
    if (close_) 
        throw std::logic_error("Node already has value"s);
        
    if (!stack.empty() && stack.back()->IsDict()) {
        if (!Dict_key.second) {
            throw std::logic_error("Key value not set"s);
        } else {
            stack.back()->AsDict().at(Dict_key.first).GetValue() = value;
            Dict_key.second = false;
            return *this;
        } 
    }

    if (!stack.empty() && stack.back()->IsArray()) {
        stack.back()->AsArray().push_back(Node());
        stack.back()->AsArray().back().GetValue() = value;
        return *this;
    }
        
    close_ = true;
    root_.GetValue() = value;
    return *this;
}
    
Node Builder::Build() {
    if (root_.IsNull()) throw std::logic_error("Node hasn't value"s);
    if (Dict_key.second) throw std::logic_error("Don't end Dict"s);
    if (!stack.empty()) throw std::logic_error("Don't end Dict or Array"s);
    return root_;
}
    
}


    