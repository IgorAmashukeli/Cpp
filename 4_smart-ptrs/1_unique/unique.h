#pragma once

#include "compressed_pair.h"

#include <cstddef>  // std::nullptr_t


// default deleter for general types
template<typename T>
struct Slug {
    Slug() = default;

    // move, copy constructors and assignments for dervived T
    template<typename U>
    Slug([[maybe_unused]] Slug<U>&& other) {}

    template<typename U>
    Slug& operator=([[maybe_unused]] Slug<U>&& other) {
        return *this;
    }

    template<typename U>
    Slug([[maybe_unused]] const Slug<U>& other) {}

    template<typename U>
    Slug& operator=([[maybe_unused]] const Slug<U>& other) {
        return *this;
    }

    ~Slug() = default;

    void operator()(T* ptr) noexcept {
        if (ptr) {
            delete ptr;
        }
    }
};

// default deleter for array-like types
template<typename T>
struct Slug<T[]> {
    Slug() = default;

    // move, copy constructors and assignments for dervived T
    // very strange - maybe wrong
    template<typename U>
    Slug([[maybe_unused]] Slug<U[]>&& other) {}

    template<typename U>
    Slug& operator= ([[maybe_unused]] Slug<U[]>&& other) {
        return *this;
    }

    template<typename U>
    Slug([[maybe_unused]] const Slug<U[]>& other) {}

    template<typename U>
    Slug& operator=([[maybe_unused]] const Slug<U[]>& other) {
        return *this;
    }

    ~Slug() = default;


    void operator()(T* ptr) noexcept {
        if (ptr) {
            delete[] ptr;
        }
    }
};

// Primary template
template <typename T, typename Deleter = Slug<T>>
class UniquePtr {
public:
    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Constructors

    explicit UniquePtr(T* ptr = nullptr) noexcept : data_(ptr, Deleter{}) {}
    UniquePtr(T* ptr, Deleter deleter) noexcept : data_(ptr, std::forward<Deleter>(deleter)) {}

    UniquePtr(UniquePtr&& other) noexcept {
        if (&other != this) {
            Swap(other);
            other.Reset();
        }
    }


    // move constructor for unique_ptr with derived types of template
    template<typename U, typename Del>
    UniquePtr(UniquePtr<U, Del>&& other) noexcept {
        this->GetDeleter() = std::move(other.GetDeleter());

        if (this->Get() != other.Get()) {
            this->Reset();
        }
        data_.GetFirst() = std::move(other.Get());
        other.Release();
    }


    // no copy constructor
    UniquePtr(const UniquePtr& other) = delete;


    ////////////////////////////////////////////////////////////////////////////////////////////////
    // `operator=`-s

    UniquePtr& operator=(UniquePtr&& other) noexcept {
        if (&other != this) {
            Swap(other);
            other.Reset();
        }
        return *this;
    }


    UniquePtr& operator=(std::nullptr_t ) noexcept {
        this->Reset();
        return *this;
    }


    // assignment operator for unique_ptr with derived types of template
    template<typename U, typename Del>
    UniquePtr& operator=(UniquePtr<U, Del>&& other) noexcept {
        this->GetDeleter() = std::move(other.GetDeleter());

        if (this->Get() != other.Get()) {
            this->Reset();
        }
        data_.GetFirst() = std::move(other.Get());
        other.Release();

        return *this;
    }



     // no copy assignment operator
    UniquePtr& operator=(const UniquePtr& other) = delete;

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Destructor

    ~UniquePtr() {
        this->Reset();
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Modifiers

    T* Release() {
        T* old_ptr = data_.GetFirst();
        data_.GetFirst() = nullptr;
        return old_ptr;

    }
    void Reset(T* ptr = nullptr) {
        auto old_ptr = data_.GetFirst();
        data_.GetFirst() = ptr;
        if (old_ptr) {
            data_.GetSecond()(old_ptr);
        }

    }
    void Swap(UniquePtr& other) {
        if (&other != this) {
            std::swap(data_, other.data_);
        }
        
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Observers

    T* Get() const {
        return data_.GetFirst();
    }
    Deleter& GetDeleter() {
        return data_.GetSecond();
    }
    const Deleter& GetDeleter() const {
        return data_.GetSecond();
    }
    explicit operator bool() const {
        return (Get() != nullptr);
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Single-object dereference operators

    template<typename U = T>
    std::enable_if_t<!std::is_void_v<U>, U&> operator*() const  {
        return *data_.GetFirst();
    }
    T* operator->() const {
        return data_.GetFirst();
    }

private:
    CompressedPair<T*, Deleter> data_ = {nullptr, Deleter{}};
};

// Specialization for arrays
template <typename T, typename Deleter>
class UniquePtr<T[], Deleter>
{
public:
    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Constructors

    explicit UniquePtr(T* ptr = nullptr) noexcept : data_(ptr, Deleter{}) {}
    UniquePtr(T* ptr, Deleter deleter) noexcept : data_(ptr, std::forward<Deleter>(deleter)) {}

    UniquePtr(UniquePtr&& other) noexcept {
        if (&other != this) {
            Swap(other);
            other.Reset();
        }
    }

     // move constructor for unique_ptr with derived types of template
    template<typename U, typename Del>
    UniquePtr(UniquePtr<U[], Del>&& other) noexcept {
        this->GetDeleter() = std::move(other.GetDeleter());

        if (this->Get() != other.Get()) {
            this->Reset();
        }
        data_.GetFirst() = std::move(other.Get());
        other.Release();
    }

    UniquePtr(const UniquePtr& other) = delete;

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // `operator=`-s

    UniquePtr& operator=(UniquePtr&& other) noexcept {
        if (&other != this) {
            Swap(other);
            other.Reset();
        }
        return *this;
    }

    UniquePtr& operator=(std::nullptr_t ) noexcept {
        this->Reset();
        return *this;
    }


    // assignment operator for unique_ptr with derived types of template
    template<typename U, typename Del>
    UniquePtr& operator=(UniquePtr<U[], Del>&& other) noexcept {
        this->GetDeleter() = std::move(other.GetDeleter());

        if (this->Get() != other.Get()) {
            this->Reset();
        }
        data_.GetFirst() = std::move(other.Get());
        other.Release();

        return *this;
    }


    // no copy assignment operator
    UniquePtr& operator=(const UniquePtr& other) = delete;
   


    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Destructor

    ~UniquePtr() {
        this->Reset();
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Modifiers

    T* Release() {
        T* old_ptr = data_.GetFirst();
        data_.GetFirst() = nullptr;
        return old_ptr;

    }
    void Reset(T* ptr = nullptr) {
        auto old_ptr = data_.GetFirst();
        data_.GetFirst() = ptr;
        if (old_ptr) {
            data_.GetSecond()(old_ptr);
        }

    }
    void Swap(UniquePtr& other) {
        if (&other != this) {
            std::swap(data_, other.data_);
        }
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Observers

    T* Get() const {
        return data_.GetFirst();
    }
    Deleter& GetDeleter() {
        return data_.GetSecond();
    }
    const Deleter& GetDeleter() const {
        return data_.GetSecond();
    }
    explicit operator bool() const {
        return (Get() != nullptr);
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Multiple-object dereference operators

    const T& operator[](size_t i) const {
        return data_.GetFirst()[i];
    }
    T& operator[](size_t i) {
        return data_.GetFirst()[i];
    }
    

private:
    CompressedPair<T*, Deleter> data_ = {nullptr, Deleter{}};
};
