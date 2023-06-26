#include "json.h"

namespace json {
    
class Builder;
    
class KeyItem;
class DictItem;
class ArrayItem;
    
class KeyItem {
private:
    Builder& builder_;
    
public :
    KeyItem(Builder& builder) : builder_(builder) {}
    
    ArrayItem StartArray();
    DictItem StartDict();
    DictItem Value(Node::Value value);
};
    
class ArrayItem {
private: 
    Builder& builder_;
    
public:
    ArrayItem(Builder& builder) : builder_(builder) {}
    
    ArrayItem StartArray();
    DictItem StartDict();
    ArrayItem Value(Node::Value value);
    Builder& EndArray();
};
    
class DictItem {
private:
    Builder& builder_;
    
public :
    DictItem(Builder& builder) : builder_(builder) {}
    
    KeyItem Key(std::string str);
    Builder& EndDict();
};
    
class Builder {
public:
    Builder() {
        Dict_key.second = false;
    }
    
    DictItem StartDict();
    KeyItem Key(std::string key);
    Builder& EndDict();
    
    ArrayItem StartArray();
    Builder& EndArray();
    
    Builder& Value(json::Node::Value value);
    
    
    Node Build();
    
private:
    Node root_;
    bool close_ = false;
    std::vector<Node*> stack;
    std::pair<std::string, bool> Dict_key;
};
    
}