#pragma once

#include <cassert>
#include <type_traits>


// default constructible
// movable, when moving, makes the other deleter have tag_ = 0
// non copyable
// works for const and non const objects
// calls "delete" for non void objects with sizeof > 0
// marks the delete as being made
template <class T>
class Deleter {
public:

    // default constructible deleter
    Deleter() = default;

    // deleter can be constructed from tag (and implicitly)
    Deleter(int tag) : tag_(tag) {
    }

    // copy constructor is deleted: you can have only one object of this type
    Deleter(const Deleter&) = delete;

    // move constructor: the other tag is changed to zero
    // exceptions can't happen here
    Deleter(Deleter&& rhs) noexcept : tag_(rhs.tag_) {
        rhs.tag_ = 0;
    }

    // copy assignment operator is deleted
    Deleter& operator=(const Deleter&) = delete;

    // move assignment operator is similiar to move constructor
    // it is also noexcept
    Deleter& operator=(Deleter&& r) noexcept {
        tag_ = r.tag_;
        r.tag_ = 0;
        return *this;
    }

    // destructor is default (the fields are simple types)
    ~Deleter() = default;

    // getter for tag
    int GetTag() const {
        return tag_;
    }

    // the actual () operator to call this functor
    // takes a pointer
    void operator()(T* p) const {
        // asserts that the oject sizeof is > 0
        // not an empty array of size 0 for example
        static_assert(sizeof(T) > 0);

        // asserts that type is not void
        static_assert(!std::is_void<T>::value);

        // actually deletes pointer
        delete p;

        // flags that it was called
        was_called_ = true;
    }

    // checks, whether deleter is const or not
    bool IsConst() const {
        return true;
    }
    bool IsConst() {
        return false;
    }

    // checks whether deleter was already called
    bool WasCalled() const {
        return was_called_;
    }

private:
    // tag can be set to non-zero only in the constructor method of the class
    int tag_ = 0;

    // mutable is used to be able to modify this field in const function
    mutable bool was_called_ = false;
};


// specialization of the above deleter for the array types
// default constructible
// movable, when moving, makes the other deleter have tag_ = 0
// non copyable
// works for const and non const objects
// calls "delete[]" for non void objects with sizeof > 0
// doesn't mark a deleted function as called
template <class T>
class Deleter<T[]> {
public:

    // default constructible deleter
    Deleter() = default;

    // deleter can be constructed from tag (and implicitly)
    Deleter(int tag) : tag_(tag) {
    }

    // no copy constructor
    Deleter(const Deleter&) = delete;

    // move constructor
    // ensures that there are no exceptions
    Deleter(Deleter&& rhs) noexcept : tag_(rhs.tag_) {
        rhs.tag_ = 0;
    }

    // copy assignment operator
    Deleter& operator=(const Deleter&) = delete;

    // move assignment operator is similiar to move constructor
    Deleter& operator=(Deleter&& r) noexcept {
        tag_ = r.tag_;
        r.tag_ = 0;
        return *this;
    }

    // default destructor
    // the type of the field are simple
    ~Deleter() = default;

    // returns the tag
    int GetTag() const {
        return tag_;
    }

    // the actual () operator to call the delete
    void operator()(T* p) const {
        // checks, whether the type has sizeof > 0
        // not the array of size 0, for example
        static_assert(sizeof(T) > 0);

        // checks that the type is not void
        static_assert(!std::is_void<T>::value);

        // deletes the pointer as an array
        // because p is the array itself
        delete[] p;
    }

    // checks, whether the object is const or not
    bool IsConst() const {
        return true;
    }
    bool IsConst() {
        return false;
    }

private:
    // tag can be sent to non-zero only in the constructor method of the class
    int tag_ = 0;
};

template <typename T>
class CopyableDeleter {
public:
    // CopyableDeleter is default constructible
    CopyableDeleter() = default;

    // CopyableDeleter can be constructed from tag
    // and implicitly
    CopyableDeleter(int tag) : tag_(tag) {
    }

    // CopyableDeleter has a default copy constructor
    // it copies the tag_, for example
    CopyableDeleter(const CopyableDeleter&) = default;

    // CopyableDeleter has move constructor
    // move constructor doesn't have any exceptions
    CopyableDeleter(CopyableDeleter&& rhs) noexcept : tag_(rhs.tag_) {
        rhs.tag_ = 0;
    }

    // CopyableDeleter has a default assignment operator
    // it copies the tag_, for example
    CopyableDeleter& operator=(const CopyableDeleter&) = default;

    // CopyableDeleter has a move assignment operator
    // which works similiarly as move
    CopyableDeleter& operator=(CopyableDeleter&& r) noexcept {
        tag_ = r.tag_;
        r.tag_ = 0;
        return *this;
    }

    // CopyableDeleter has a destructor
    ~CopyableDeleter() = default;

    // getter for tag
    int GetTag() const {
        return tag_;
    }

    void operator()(T* p) const {
        // asserts that that the type has not zero size
        static_assert(sizeof(T) > 0);

        // asserts that the the type is not void
        static_assert(!std::is_void<T>::value);

        // deletes the pointer
        // general implementation of the deleter
        // (not for array type objects)
        delete p;
    }

    // checks, whether the type is const or not
    bool IsConst() const {
        return true;
    }
    bool IsConst() {
        return false;
    }

private:
    // tag
    int tag_ = 0;
};
