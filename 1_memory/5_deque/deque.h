#pragma once

#include <initializer_list>
#include <algorithm>
#include <cassert>
#include <iostream>
#include <ostream>


// main constants
enum BLOCK {
    SIZE = 128
};



// Block abstraction
class Block {
public:
    
    // all fields are already assigned to correct values
    Block() = default;
    
    // Block copy constructor
    Block(const Block& other) {
        *this = other;
    }

    // Block copy assignment operator
    Block& operator=(const Block& other) {
        // nothing to do, it is self-assignment
        if (this == &other) {
            return *this;
        }

        // nothing to do
        if (other.data_ == nullptr && data_ == nullptr) {
            return *this;
        }

        // deallocate this
        if (other.data_ == nullptr) {
            DeAllocate();
            l_ = BLOCK::SIZE;
            r_ = -1;
            return *this;
        }

       

        if (!is_allocated_) {
            Allocate();
        }

        // copy l_, r_ and is_allocated_
        l_ = other.l_;
        r_ = other.r_;
        is_allocated_ = other.is_allocated_;

        // assert this are correct int
        assert(l_ >= 0 && r_ >= 0);

        // copy the range
        std::copy(other.data_ + l_, other.data_ + r_ + 1, data_ + l_);

        return *this;
    }
    
    // std::move constructor
    Block(Block&& other) {
        // call std::move assignment operator
        *this = std::move(other);
    }
    
    // std::move assignment operator
    Block& operator=(Block&& other) {

        // note that despite the fact that
        // when this != &other
        // all the elements pointers are not invalidated during the std::move operation
        Swap(other);
        
        return *this;
    }
    
    // swap function for the block
    void Swap(Block& other) {
        std::swap(data_, other.data_);
        std::swap(is_allocated_, other.is_allocated_);
        std::swap(l_, other.l_);
        std::swap(r_, other.r_);
    }
    
    
    // allocate memory for the Block (back or front)
    void Allocate() {
        assert(!is_allocated_);
        
        // memory allocation
        data_ = new int[BLOCK::SIZE];
        
        // setting up the allocation flag
        is_allocated_ = true;
    }
    
    // deallocate memory for the Block
    void DeAllocate() {
        // the array can't be allready deallocated
        assert(is_allocated_);
            
        // memory deallocation
        delete[] data_;
        data_ = nullptr;
        
        // setting up the allocation flag
        is_allocated_ = false;
        
    }
    
    // fill the k first values of the Block
    void Fill(size_t k) {
        if (IsEmpty()) {
            Allocate();
        }
        // we can't fill back 0 value (there is no need to create such block)
        // we can't fill more than BLOCK::SIZE
        // assert it is allocated_
        assert (k > 0 && k <= BLOCK::SIZE && is_allocated_);
        
        // fill the values
        std::fill_n(data_, k, 0);
        
        // std::move the indicies
        l_ = 0;
        r_ = k - 1;
    }

    // push_back
    void PushBack(int x) {
        // we can't push_back, if the array is filled
        assert(!IsBackFilled());
        
        if (IsEmpty()) {
            l_ = 0;
            Allocate();
        }
        
        // put the value in the array
        data_[r_ + 1] = x;
        
        // std::move the index
        ++r_;
    }

    // push_front
    void PushFront(int x) {
        // we can't push_back, if the array is filled
        assert(!IsFrontFilled());

        if (IsEmpty()) {
            r_ = BLOCK::SIZE - 1;
            Allocate();
        }
        
        
        // put the value in the array
        data_[l_ - 1] = x;
        
        // std::move the index
        --l_;
    }

    // pop_front
    void PopBack() {
        // Block can't be empty or be the wrong type
        assert(!IsIndexEmpty());
        
        // std::move the index
        --r_;
        
        // if array is empty
        // deallocate
        if (IsIndexEmpty()) {
            DeAllocate();
            l_ = BLOCK::SIZE;
            r_ = -1;
        }
    }

    void PopFront() {
        // Block can't be empty or be the wrong type
        assert(!IsIndexEmpty());
        
        // std::move the index
        ++l_;
        
        // if array is empty
        // deallocate
        if (IsIndexEmpty()) {
            DeAllocate();
            l_ = BLOCK::SIZE;
            r_ = -1;
        }
    }
    
    bool IsFrontFilled() const {
        return l_ == 0;
    }
    
    bool IsBackFilled() const {
        return (r_ + 1 == BLOCK::SIZE);
    }

    // checks if array is filled
    bool IsFilled() const {
        return (IsFrontFilled() && IsBackFilled());
    }

    // checks if array is empty
    // this is called before changing the type to BLOCK::EMPTY
    bool IsIndexEmpty() const {
        return (l_ > r_);
    }

    // checks if array has empty type and therefore is already empty
    bool IsEmpty() const {
        return !is_allocated_;
    }

    // not const operator[]
    int& operator[] (size_t i) {
        assert(l_ >= 0 && r_ >= 0);
        assert(!IsEmpty() && static_cast<int>(i) >= l_ && static_cast<int>(i) <= r_);
        return data_[i];
    }

    // const operator[]
    int operator[] (size_t i) const {
        assert(l_ >= 0 && r_ >= 0);
        assert(!IsEmpty() && static_cast<int>(i) >= l_ && static_cast<int>(i) <= r_);
        return data_[i];
    }
    
    // size
    size_t Size() const {
        if (IsEmpty()) {
            return 0;
        }
        assert (l_ <= r_);
        return r_ - l_ + 1;
    }
    
    // get l_ index
    size_t GetL() const {
        return l_;
    }
    
    // get r_ index
    size_t GetR() const {
        return r_;
    }
    
    ~Block() {
        
        // if array is allocated, deallocate
        if (is_allocated_) {
            DeAllocate();
        }
        
        // change l and r indicies
        l_ = BLOCK::SIZE;
        r_ = -1;
    }

private:
    bool is_allocated_ = false;
    int* data_ = nullptr;
    int l_ = BLOCK::SIZE;
    int r_ = -1;
};


class Deque {
public:

    // all fields are already assigned to the correct values
    Deque() = default;

    // copy constructor
    Deque(const Deque& other) {
        *this = other;
    }

    // copy assignment operator
    Deque& operator=(const Deque& other) {
        // nothing to do, it is self-assignment
        if (this == &other) {
            return *this;
        }

        // nothing to do
        if (other.buffer_ == nullptr && buffer_ == nullptr) {
            return *this;
        }

        // restd::move everything
        Clear();

        // nothing to do
        if (other.buffer_ == nullptr) {
            return *this;
        }

        // allocate new buffer
        buffer_ = new Block[other.buffer_cap_];

        // copy all the blocks
        std::copy(other.buffer_, other.buffer_ + other.buffer_cap_, buffer_);

        // reassign the fields
        buffer_cap_ = other.buffer_cap_;
        buffer_size_ = other.buffer_size_;
        buffer_begin_ = other.buffer_begin_;

        return *this;
    }

    // std::move constructor
    Deque(Deque&& other) {
        *this = std::move(other);
    }
    
    // std::move assignment operator
    Deque& operator=(Deque&& other) {
        // note that despite the fact that
        // when this != &other
        // all the elements pointers are not invalidated during the std::move operation
        Swap(other);
        
        return *this;
    }
    
    
    // swap all fields
    void Swap(Deque& other) {
        std::swap(buffer_, other.buffer_);
        std::swap(buffer_cap_, other.buffer_cap_);
        std::swap(buffer_size_, other.buffer_size_);
        std::swap(buffer_begin_, other.buffer_begin_);
    }
    
    void ReAllocate() {
        // if buffer_cap_ = 0
        // we need to allocate, not to reallocate
        assert(buffer_cap_ > 0);
        
        // if buffer_size_ < buffer_cap_
        // no need to reallocate
        assert(buffer_cap_ == buffer_size_);
        
        // if front is not frontfilled and back is not backfilled
        // no need to reallocate
        assert(Front().IsFrontFilled() || Back().IsBackFilled());
        
        // create new buffer of size two times higher
        Block* new_buffer_ = new Block[buffer_cap_ * 2];
        
        // std::move all the memory from the buffer blocks to the new buffer
        for (size_t i = 0; i < buffer_size_; ++i) {
            new_buffer_[i] = std::move(buffer_[GetInd(i)]);
        }
        
        // delete the previous memory
        delete[] buffer_;
        
        // reassign the fields
        buffer_ = new_buffer_;
        
        buffer_cap_ *= 2;
        buffer_begin_ = 0;
    }

    // allocate the first block for the PushBack/PushFront
    void AllocateFirstBlock() {
        buffer_ = new Block[1];
    }

    // allocate this number of back blocks to fill for the constructor
    void AllocateFillBlocks(size_t count) {
        buffer_ = new Block[count];
    }

    size_t GetInd(size_t i) const {
        return (buffer_begin_ + i) % buffer_cap_;
    }

    Block& At(size_t i) {
        // buffer capacity can't be zero for this function
        assert(buffer_cap_ > 0);
        return buffer_[GetInd(i)];
    }

    const Block& At(size_t i) const {
        // buffer capacity can't be zero for this function
        assert(buffer_cap_ > 0);
        return buffer_[GetInd(i)];
    }

    Block& Front() {
        return At(0);
    }

    const Block& Front() const {
        return At(0);
    }

    Block& Back() {
        // buffer size can't be zero for this function
        assert(buffer_size_ > 0);
        return At(buffer_size_ - 1);
    }

    const Block& Back() const {
        // buffer size can't be zero for this function
        assert(buffer_size_ > 0);
        return At(buffer_size_ - 1);
    }

    // get the number of blocks from size
    size_t CalcBlockCount(size_t size) const {
        // remainder
        size_t rem = size % BLOCK::SIZE;

        // block count
        size_t count = size / BLOCK::SIZE;
        count += static_cast<size_t>(rem > 0);

        return count;
    }

    explicit Deque(size_t size) {
        // calculate the number of blocks and allocate so many
        size_t count = CalcBlockCount(size);
        AllocateFillBlocks(count);

        // filling with zeroes
        for (size_t i = 0; i < count; ++i) {

            size_t cur_size = BLOCK::SIZE;

            // the last block can be not full
            if ((i + 1 == count) && (size % BLOCK::SIZE > 0)) {
                cur_size = size % BLOCK::SIZE;
            }


            buffer_[i].Fill(cur_size);
        }

        // assign the correct values to the other fields
        buffer_cap_ = count;
        buffer_begin_ = 0;
        buffer_size_ = count;
    }

    Deque(std::initializer_list<int> list) {
        // calculate the number of blocks and allocate so many
        size_t count = CalcBlockCount(list.size());
        AllocateFillBlocks(count);

        // filling with list
        size_t i = 0;
        for (int x : list) {
            buffer_[i].PushBack(x);

            if (buffer_[i].IsFilled()) {
                ++i;
            }
        }

        // assign the correct values to the other fields
        buffer_cap_ = count;
        buffer_begin_ = 0;
        buffer_size_ = count;
    }

    // push value in the empty deque
    void PushFirst(int value, bool block_type) {

       // allocate memory 
        AllocateFirstBlock();

        // push value and change the number of according blocks
        if (block_type == 1) {
            buffer_[0].PushBack(value);
            buffer_size_ = 1;
        } else {
            buffer_[0].PushFront(value);
            buffer_size_ = 1;
        }
        // change the capacity
        buffer_cap_ = 1;
    }

    void PushBack(int value) {
        // if deque is empty, push the first value
        if (buffer_cap_ == 0) {
            PushFirst(value, true);
            return;
        }

        // the back block
        Block& back = Back();

        // there is a place in the Back
        // push_back
        if (!back.IsBackFilled()) {
            back.PushBack(value);
            return;
        }

        // index of the next to the back can't exceed capacity
        assert(GetInd(buffer_size_) < buffer_cap_);

        // get the block after the back
        Block& next_back_block = At(buffer_size_);

        // if there are at least two blocks
        // if the next block is not the first one
        // and the next block is not back filled
        if (buffer_cap_ > 1 && 
        (GetInd(buffer_size_) != GetInd(0)) && 
        (!(next_back_block.IsBackFilled()))) {
            
            // if it is empty
            if (next_back_block.IsEmpty()) {
                ++buffer_size_;
            }
            next_back_block.PushBack(value);
            return;
        }

        // otherwise we need to reallocate
        ReAllocate();

        // change the buffer size
        ++buffer_size_;

        // now we can push_back the value
        PushBack(value);
    }

    void PopBack() {
        // assert size is not zero
        assert(buffer_size_ != 0);

        // the back block
        Block& back = Back();

        // assert back is not empty
        // back is the correct last non-empty block
        // it can't be empty
        assert(!back.IsEmpty());

        // we can restd::move one element
        back.PopBack();

        // if it is again not empty, nothing to do
        if (!back.IsEmpty()) {
            return;
        }

        // if size was not 1
        // there is at least one block prior to Back
        if (buffer_size_ > 1) {
            
            // decrease the number of blocks
            --buffer_size_;

            return;
        }

        // size was 1
        // restd::move everything
        Clear();
    }

    void PushFront(int value) {
        // if deque is empty, push the first value
        if (buffer_cap_ == 0) {
            PushFirst(value, false);
            return;
        }

        // the front block
        Block& front = Front();

        // there is a place in the Front
        // push_front
        if (!front.IsFrontFilled()) {
            front.PushFront(value);
            return;
        }

        // index of the prev to the front can't exceed capacity
        assert(GetInd(buffer_cap_ - 1) < buffer_cap_);

        // get the block prev to the front
        Block& prev_front_block = At(buffer_cap_ - 1);

        // if there are at least two blocks
        // if the prev block is not the last one
        // and the prev block is not back filled
        // push into that block
        if (buffer_cap_ > 1 && 
        (GetInd(buffer_cap_ - 1) != GetInd(buffer_size_ - 1)) && 
        (!(prev_front_block.IsFrontFilled()))) {
            

            // if it is empty
            if (prev_front_block.IsEmpty()) {
                // change the buffer size
                ++buffer_size_;

                // change the begining to that index
                buffer_begin_ = GetInd(buffer_cap_ - 1);
            }

            prev_front_block.PushFront(value);

            return;
        }

        // otherwise we need to reallocate
        ReAllocate();

        // change the buffer size
        ++buffer_size_;

        // change the begining to that index
        buffer_begin_ = GetInd(buffer_cap_ - 1);

        // now we can push_front the value
        PushFront(value);
    }

    void PopFront() {
        // assert size is not zero
        assert(buffer_size_ != 0);

        // the front block
        Block& front = Front();

        // assert front is not empty
        // front is the correct first non-empty block
        // it can't be empty
        assert(!front.IsEmpty());

        // we can restd::move one element
        front.PopFront();

        // if it is again not empty, nothing to do
        if (!front.IsEmpty()) {
            return;
        }


        // if size was not 1
        // there is at least one block next to Front
        if (buffer_size_ > 1) {
            
            // decrease the number of blocks
            --buffer_size_;

            // change begin
            ++buffer_begin_;

            return;
        }

        // size was 1
        // restd::move everything
        Clear();
    }


    int operator[] (size_t i) const {

        // this can't be called if size is zero
        assert(buffer_size_ > 0);

        // index can't be >= than i
        assert(i < Size());

        // if there is only one block or i is less than the size of the first block
        if (buffer_size_ == 1 || i < Front().Size()) {
            const Block& front = Front();
            size_t l_ = front.GetL();
            return front[l_ + i];
        }

        // skip all the elements in the first block 
        i -= Front().Size();

        // get the block number
        size_t block_num = 1 + i / BLOCK::SIZE;

        // get the block place
        size_t place = i % BLOCK::SIZE;

        return At(block_num)[place];
    }

    int& operator[] (size_t i) {
        // this can't be called if size is zero
        assert(buffer_size_ > 0);

        // index can't be >= than i
        assert(i < Size());

        // if there is only one block or i is less than the size of the first block
        if (buffer_size_ == 1 || i < Front().Size()) {
            Block& front = Front();
            size_t l_ = front.GetL();
            return front[l_ + i];
        }

        // skip all the elements in the first block 
        i -= Front().Size();

        // get the block number
        size_t block_num = 1 + i / BLOCK::SIZE;

        // get the block place
        size_t place = i % BLOCK::SIZE;

        return At(block_num)[place];
    }


    size_t Size() const {
        // no blocks => no size
        if (buffer_size_ == 0) {
            return 0;
        }

        // one block => get the size of this block
        if (buffer_size_ == 1) {
            return Front().Size();
        }

        // there are at least two blocks
        // all the blocks in the middle are fully filled
        size_t size = (buffer_size_ - 2) * BLOCK::SIZE;
        
        // add the size of the most left and most right block
        return size + Front().Size() + Back().Size();
    }

    // clears all the memory
    void Clear() {
        if (buffer_cap_ == 0) {
            return;
        }
        
        // this should never happen in case of non-empty capacity
        assert(buffer_ != nullptr);

        // delete will
        // 1) call destructors for Block objects
        //    these destructors will deallocate memory for each block, if needed
        // 2) will restd::move all the memory, allocated for the buffer_ array
        delete[] buffer_;

        buffer_ = nullptr;
        buffer_cap_ = 0;
        buffer_begin_ = 0;
        buffer_size_ = 0;
    }

    // call clear function to deallocate all memory
    ~Deque() {
        Clear();
    }

private:
    // capacity of the deque
    size_t buffer_cap_ = 0;

    // block, where it begins
    size_t buffer_begin_ = 0;

    // number of blocks
    size_t buffer_size_ = 0;

    // buffer
    Block* buffer_ = nullptr;
};

