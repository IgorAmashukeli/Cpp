#pragma once

#include <string>
#include <unordered_map>
#include <list>

class LruCache {
public:
    LruCache(size_t max_size);

    void Set(const std::string& key, const std::string& value);

    bool Get(const std::string& key, std::string* value);

private:

    // places of the keys 
    std::unordered_map<std::string, std::list<std::pair<std::string, std::string>>::iterator> its_;

    // the values
    std::list<std::pair<std::string, std::string>> values_;
    size_t max_size_;
};
