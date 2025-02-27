#pragma once
#include "sw_fwd.h"  // Forward declaration

#include <cstddef>  // std::nullptr_t
#include <memory>

class ControlBlock {
public:
    // virtual control block destructor
    virtual ~ControlBlock() = default;

    size_t GetCounter() {
        return counter_;
    }

    void Inc() {
        ++counter_;
    }

    // decrement function
    void Dec() {
        if (counter_ > 1) {
            --counter_;
        } else {
            delete this;
        }
    }

private:
    size_t counter_ = 1;
};

// control block, when block stores pointer
template <typename Y>
class ControlBlockPtr : public ControlBlock {
public:
    ControlBlockPtr(Y* y_ptr) : y_ptr_(y_ptr) {};

    ~ControlBlockPtr() {
        delete y_ptr_;
    }

private:
    Y* y_ptr_;
};

// control block, when block stores object
template <typename Y>
class ControlBlockObject : public ControlBlock {
public:
    template <typename... Args>
    ControlBlockObject(Args&&... args) : y_(std::forward<Args>(args)...) {
    }

    Y* GetY() {
        return &y_;
    }

    ~ControlBlockObject() = default;

private:
    Y y_;
};

// https://en.cppreference.com/w/cpp/memory/shared_ptr
template <typename T>
class SharedPtr {
public:
    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Constructors

    // default constructor is the same as std::nullptr_t constructor
    // it doesn't change anything
    SharedPtr() noexcept = default;
    SharedPtr(std::nullptr_t) noexcept {
    }

    // control block of Y, observing T*
    // can have Y = T
    template <typename Y>
    explicit SharedPtr(Y* ptr) {
        ptr_ = ptr;
        block_ = new ControlBlockPtr<Y>(ptr);
    }

    template <typename Y>
    SharedPtr(Y* ptr, ControlBlockObject<Y>* block) {
        ptr_ = ptr;
        block_ = block;
    }

    SharedPtr(const SharedPtr& other) noexcept {
        if (other.block_ != nullptr) {
            ptr_ = other.ptr_;
            block_ = other.block_;
            block_->Inc();
        }
    }

    template <typename Y>
    SharedPtr(const SharedPtr<Y>& other) noexcept {
        if (other.block_ != nullptr) {
            ptr_ = other.ptr_;
            block_ = std::decay_t<ControlBlock*>(other.block_);
            block_->Inc();
        }
    }

    SharedPtr(SharedPtr&& other) noexcept {
        Swap(other);
    }

    template <typename Y>
    SharedPtr(SharedPtr<Y>&& other) {
        if (other.block_ != nullptr) {
            ptr_ = other.ptr_;
            block_ = other.block_;
            other.ptr_ = nullptr;
            other.block_ = nullptr;
        }
    }

    // Aliasing constructor
    // #8 from https://en.cppreference.com/w/cpp/memory/shared_ptr/shared_ptr
    template <typename Y>
    SharedPtr(const SharedPtr<Y>& other, T* ptr) noexcept {
        ptr_ = ptr;
        if (other.block_ != nullptr) {
            block_ = other.block_;
            block_->Inc();
        }
    }

    // Promote `WeakPtr`
    // #11 from https://en.cppreference.com/w/cpp/memory/shared_ptr/shared_ptr
    explicit SharedPtr(const WeakPtr<T>& other);

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // `operator=`-s

    SharedPtr& operator=(const SharedPtr& other) noexcept {
        if (this != &other) {
            if (block_ != other.block_) {
                Reset();
                if (other.block_ != nullptr) {
                    block_ = other.block_;
                    block_->Inc();
                }
            }
            ptr_ = other.ptr_;
        }

        return *this;
    }

    SharedPtr& operator=(SharedPtr&& other) noexcept {
        if (this != &other) {
            Swap(other);
            other.Reset();
        }

        return *this;
    }

    template <typename Y>
    SharedPtr& operator=(const SharedPtr<Y>& other) noexcept {
        if (block_ != other.block_) {
            Reset();
            if (other.block_ != nullptr) {
                block_ = other.block_;
                block_->Inc();
            }
        }
        ptr_ = other.ptr_;

        return *this;
    }

    template <typename Y>
    SharedPtr& operator=(SharedPtr<Y>&& other) noexcept {
        if (block_ != other.block_) {
            Reset();
            if (other.block_ != nullptr) {
                block_ = other.block_;
            }
        }
        ptr_ = other.ptr_;
        other.ptr_ = nullptr;
        other.block_ = nullptr;

        return *this;
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Destructor

    ~SharedPtr() {
        Reset();
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Modifiers

    void Reset() {
        if (block_) {
            block_->Dec();
            ptr_ = nullptr;
            block_ = nullptr;
        }
    }

    // ptr is not owned (otherwise - UB - as in STL)
    void Reset(T* ptr) {
        Reset();
        if (ptr != nullptr) {
            ptr_ = ptr;
            block_ = new ControlBlockPtr<T>(ptr_);
        }
    }

    // ptr is not owned (otherwise - UB - as in STL)
    template <typename Y>
    void Reset(Y* ptr) {
        Reset();
        if (ptr != nullptr) {
            ptr_ = ptr;
            block_ = new ControlBlockPtr<Y>(ptr);
        }
    }

    void Swap(SharedPtr& other) {

        // swaping ptr and block
        std::swap(ptr_, other.ptr_);
        std::swap(block_, other.block_);
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Observers

    // observers return ptr_
    T* Get() const {
        return ptr_;
    }
    T& operator*() const {
        return *ptr_;
    }
    T* operator->() const {
        return ptr_;
    }
    size_t UseCount() const {
        if (!block_) {
            return 0;
        }

        return block_->GetCounter();
    }
    explicit operator bool() const {
        return (ptr_ != nullptr);
    }

private:
    // adding friend class SharedPtr
    // it is done to access private fields of SharedPtr<Y> for different Y
    template <typename Y>
    friend class SharedPtr;

    // sizeof will be 16 as in shared_ptr (two pointers of sizeof 8)

    // observed pointer
    T* ptr_ = nullptr;

    // block of owned memory
    ControlBlock* block_ = nullptr;
};

template <typename T, typename U>
inline bool operator==(const SharedPtr<T>& left, const SharedPtr<U>& right);

// Allocate memory only once
template <typename T, typename... Args>
SharedPtr<T> MakeShared(Args&&... args) {
    ControlBlockObject<T>* block = new ControlBlockObject<T>(std::forward<Args>(args)...);

    T* ptr = block->GetY();
    return SharedPtr<T>(ptr, block);
}