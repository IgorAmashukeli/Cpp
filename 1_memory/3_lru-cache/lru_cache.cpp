#include "lru_cache.h"

LruCache::LruCache(size_t max_size) {
    max_size_ = max_size;
}


void LruCache::Set(const std::string& key, const std::string& value) {
    // check, if it contains the key
    if (its_.contains(key)) {
        // find the place
        auto it = its_[key];

        // erase from the place and put it to the back - the most recently used
        values_.erase(it);
        values_.push_back({key, value});

        // save the new position of the key-value
        its_[key] = std::prev(values_.end());
    } else {

        // put it to the back - the most recently used
        values_.push_back({key, value});

        // save the new position of the key-value
        its_[key] = std::prev(values_.end());
    }

    // remove the least used value
    if (values_.size() > max_size_) {
        // erase the key place of the key, that can be found at the front of the list
        its_.erase(values_.front().first);

        // remove the value from the list
        values_.pop_front();
    }
}

bool LruCache::Get(const std::string& key, std::string* value) {
    // check if it contains the key
    if (its_.contains(key)) {

        // find the place, copy the string to the value pointer
        auto it = its_[key];
        *value = it->second;

        // erase from the place and put it to the back - the most recently used
        values_.erase(it);
        values_.push_back({key, *value});

        // save the new position of the key-value
        its_[key] = std::prev(values_.end());
        return true;
    }
    return false;
}
