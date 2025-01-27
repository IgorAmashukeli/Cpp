#include "cow_vector.h"
#include <cassert>

// empty constructor
COWVector::COWVector() {

    state_ = new State{};

}


// link to another state
void COWVector::Link(State* other_state) {

    // number of references can't be less than 1
    assert(other_state->ref_count >= 1);

    // reassign the state
    state_ = other_state;

    // increment the number of references
    ++(state_->ref_count);

}


// remove the number of references or delete the state
void COWVector::UnLink() {

    // nothing to do if unlinked
    if (!state_) {
        return;
    }

    // number of references can't be less than 1
    assert(state_->ref_count >= 1);

    // if it is the last owner of the memory
    // delete it and make it nullptr
    if (state_->ref_count == 1) {
        delete state_;
        state_ = nullptr;


    // if it is not the last owner of the memory
    // decrement the number of references to the vector
    } else {
        --state_->ref_count;
    }
}


// deep copy function
void COWVector::DeepCopy() {

    // number of refences can't be less than 1
    assert(state_->ref_count >= 1);

    // if the reference count is 1, we should not do anything
    // the only owner of the vector is the current COWVector
    // we can change it freely;
    if (state_->ref_count == 1) {
        return;
    }


    // if the reference count is bigger than 1

    // we should remember the link to the vector in the current state
    const std::vector<std::string>& old_vec = state_->vec;

    // we should unlink this COWVector from the current state
    UnLink();

    // we should create new state
    state_ = new State{};

    // we should copy the vector from the previous state to the new state
    state_->vec = old_vec;
}

// copy constructor
COWVector::COWVector(const COWVector& other) {

    // link to the state of the other object
    Link(other.state_);

}


// copy assignment operator
COWVector& COWVector::operator=(const COWVector& other) {
    // self-assignment, nothing to do
    if (this == &other) {
        return *this;
    }

    // if this COWVector owns the same memory as the other one
    // nothing to do
    if (state_ == other.state_) {
        return *this;
    }

    // if this COWVector owns different memory
    // unlink this memory
    UnLink();
    

    // link to another memory
    Link(other.state_);


    return *this;
}



// move assignment operator



// size of the vector
size_t COWVector::Size() const {

    return (state_->vec).size();

}

// resize, it does the deep copy and then the resize itself
void COWVector::Resize(size_t size) {
    // do the deep copy
    DeepCopy();

    // do the resize itself
    (state_->vec).resize(size);

}

// may be there is need to add const qualifier later
// get the string at position at
const std::string& COWVector::Get(size_t at) const {
    
    return (state_->vec)[at];

}

// may be there is need to add const qualifier later
// get the last string
const std::string& COWVector::Back() const {

    return (state_->vec).back();
}

// push_back, it does the deep copy and then the push_back itself
void COWVector::PushBack(const std::string& value) {
    // do the deep copy
    DeepCopy();

    // do the push_back itself
    (state_->vec).push_back(value);
}

// set, it does the deep copy and then changes the value itself
void COWVector::Set(size_t at, const std::string& value) {
    
    // do the deep copy
    DeepCopy();

    // do the set itself
    (state_->vec)[at] = value;

}


// destructor of the cow vector
COWVector::~COWVector() {
    UnLink();
}
