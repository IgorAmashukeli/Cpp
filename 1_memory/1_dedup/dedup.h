#pragma once

#include <memory>
#include <vector>
#include <string>
#include <unordered_map>

using std::string;

std::vector<std::unique_ptr<string>> Duplicate(const std::vector<std::shared_ptr<string>>& items) {
    
    // result vector
    std::vector<std::unique_ptr<string>> out;

    // fill the vector
    for (size_t i = 0; i < items.size(); ++i) {
        out.push_back(make_unique<string>(*items[i]));
    }

    return out;
}

std::vector<std::shared_ptr<string>> DeDuplicate(
    const std::vector<std::unique_ptr<string>>& items) {
    
    // result vector
    std::vector<std::shared_ptr<string>> out;

    // indicies, where the string was met
    std::unordered_map<string, size_t> indicies;

    // fill the vector
    for (size_t i = 0; i < items.size(); ++i) {
        if (indicies.contains(*items[i])) {
            out.push_back(out[indicies[*items[i]]]);
        } else {
            out.push_back(make_shared<string>(*items[i]));
            indicies[*items[i]] = i;
        }
        
    }

    return out;
}
