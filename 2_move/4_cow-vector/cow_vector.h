#pragma once

#include <cstdint>
#include <string>
#include <vector>

struct State {

    int ref_count = 1;

    std::vector<std::string> vec;

};


class COWVector {
public:
    COWVector();
    ~COWVector();

    COWVector(const COWVector& other);
    COWVector& operator=(const COWVector& other);


   
    COWVector(COWVector&& other) = delete;
    COWVector& operator=(COWVector&& other) = delete;

    size_t Size() const;

    void Resize(size_t size);

    // very strange that there is no const qualifier
    // this function can be called on the const object
    // there are no const objects in the tests
    // however, we may want to change this in the future
    const std::string& Get(size_t at) const;
    const std::string& Back() const;

    void PushBack(const std::string& value);

    void Set(size_t at, const std::string& value);

private:

    // used to link to another state
    void Link(State* other_state);

    // used to remove the number of references or delete the state
    void UnLink();

    // used to do the deep copy
    void DeepCopy();

    State* state_;
};
