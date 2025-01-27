#pragma once


// uses Iterator as a template
template <class Iterator>

// range of an Iterator
class IteratorRange {
public:

    // saving begin and end of the iterator
    IteratorRange(Iterator begin, Iterator end) : begin_(begin), end_(end) {
    }

    // returns begin()
    Iterator begin() const {
        return begin_;
    }

    // returns end()
    Iterator end() const {
        return end_;
    }

private:
    Iterator begin_, end_;
};


// iterator for range function
template<typename Integer1, typename Integer2>
class RangeIterator {
public:
    // range iterator constructor
    RangeIterator(Integer1 val, Integer2 step) : val_(val), step_(step) {};

    // default creation of the copy and move
    // constructors and assignment operators
    // default creation of the default constructor
    // rule of 0
    


    // prefix ++
    RangeIterator& operator++() {
        // shift the value by the step
        // we moved to the next value that iterator should point to
        val_ += static_cast<Integer1>(step_);

        // return *this
        return *this;
    }


    // get the value
    Integer1 operator* () const {
        return val_;
    }


    bool operator == (const RangeIterator<Integer1, Integer2>& other) const {
        // val >= other.val
        // if val < other.val
        return (val_ >= other.val_ && step_ == other.step_);
    }

    bool operator != (const RangeIterator<Integer1, Integer2>& other) const {
        return ! ((*this) == other);
    }

private:
    Integer1 val_;
    Integer2 step_;
};


IteratorRange<RangeIterator<int64_t, int64_t>> Range(int64_t from, int64_t to, int64_t step = 1) {
    RangeIterator<int64_t, int64_t> begin(from, step);
    RangeIterator<int64_t, int64_t> end(to, step);

    return IteratorRange<RangeIterator<int64_t, int64_t>>(begin, end);
}

IteratorRange<RangeIterator<int64_t, int64_t>> Range(int64_t to) {
    return Range(0, to);
}



typename Iterator1, typename Iterator2>
class ZipIterator {
public:
    ZipIterator(Iterator1 it1, Iterator2 it2) :
     it1_(it1), it2_(it2) {}
        
    
    ZipIterator& operator++() {
        ++it1_;
        ++it2_;
        return *this;
    }

    template<typename F, typename S>
    std::pair<F, S> operator* () {
        return {*it1_, *it2_};
    }

    
private:
    Iterator1 it1_;
    Iterator2 it2_;
};
