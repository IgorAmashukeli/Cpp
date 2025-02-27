#pragma once


#include <iostream>
#include <utility>
#include <cassert>
#include <type_traits>
#include <cstdint>
#include <algorithm>

template <class Iterator>
class IteratorRange {
public:
    IteratorRange(Iterator begin, Iterator end) : begin_(begin), end_(end) {
        
    }


    using const_iterator = Iterator;


    Iterator begin() const {
        return begin_;
    }

    Iterator end() const {
        return end_;
    }

private:
    Iterator begin_, end_;
};


// assuming Integer2 has range less than Integer1
// because the last value of Integer2 and the first value of Integer2 differ as
// the value of Integer1

// the biggest value of Integer1 should be smaller than difference between
// the biggest and smallest Integer1





template<typename Integer1, typename Integer2>
class RangeIterator {
public:
    RangeIterator(Integer1 val, Integer2 step) : val_(val), step_(step) {
        assert(std::is_signed<Integer1>::value);
        assert(std::is_signed<Integer2>::value);
    }

    // default creation of the copy and move
    // constructors and assignment operators
    // default creation of the default constructor
    // rule of 0

    RangeIterator& operator++ () {
        val_ += static_cast<Integer1>(step_);
        return *this;
    }

    Integer1 operator* () const {
        return val_;
    }

    bool operator != (const RangeIterator& other) const {
        assert (other.step_ == this->step_);
        return this->val_ < other.val_;
    }


private:
    Integer1 val_;
    Integer2 step_;
};


template<typename Integer1, typename Integer2>
IteratorRange<RangeIterator<Integer1, Integer2>> Range(Integer1 from, Integer1 to, Integer2 step) {
    RangeIterator<Integer1, Integer2> begin(from, step);
    RangeIterator<Integer1, Integer2> end(to, step);

    return IteratorRange<RangeIterator<Integer1, Integer2>>(begin, end);
}


template<typename Integer1>
IteratorRange<RangeIterator<Integer1, Integer1>> Range(Integer1 from, Integer1 to) {
    return Range<Integer1, Integer1>(from, to, 1);
}


template<typename Integer1>
IteratorRange<RangeIterator<Integer1, Integer1>> Range(Integer1 to) {
    return Range<Integer1>(0, to);
}



template<typename Iterator1, typename Iterator2>
class ZipIterator {
public:
    ZipIterator(Iterator1 it_1, Iterator2 it_2) : it1_(it_1), it2_(it_2) {}

    // default creation of the copy and move
    // constructors and assignment operators
    // default creation of the default constructor
    // rule of 0

    ZipIterator& operator++ () {
        ++it1_;
        ++it2_;
        return *this;
    }

    auto operator* () const {
        return std::make_pair(*it1_, *it2_);
    }

    bool operator != (const ZipIterator& other) const {

        return (it1_ != other.it1_) && (it2_ != other.it2_);
    }

private:
    Iterator1 it1_;
    Iterator2 it2_;
};



template<typename Iterator>
class GroupIterator {
public:

    Iterator FindFinish(Iterator start) const {
        if (start != seq_end_) {
            return std::find_if(start, seq_end_, [&](const auto& q) {return !(q == *start);});
        } else {
            return seq_end_;
        }
    }

    GroupIterator(Iterator start, Iterator seq_end) : start_(start), seq_end_(seq_end) {
        finish_ = FindFinish(start);
    }

    GroupIterator& operator++ () {
        start_ = finish_;
        finish_ = FindFinish(start_);
        return *this;
    }

    IteratorRange<Iterator> operator* () const {
        return IteratorRange<Iterator>(start_, finish_);
    }

    bool operator != (const GroupIterator& other) {
        return start_ != other.start_;
    }

private:
    Iterator start_;
    Iterator finish_;
    Iterator seq_end_;
};


template<typename Iterative>
IteratorRange<GroupIterator<typename Iterative::const_iterator>> Group(const Iterative& seq) {
    GroupIterator<typename Iterative::const_iterator> begin_it(seq.begin(), seq.end());
    GroupIterator<typename Iterative::const_iterator> end_it(seq.end(), seq.end());
    return IteratorRange<GroupIterator<typename Iterative::const_iterator>>(begin_it, end_it);
}



template<typename Iterative1, typename Iterative2>
IteratorRange<ZipIterator<typename Iterative1::const_iterator, typename Iterative2::const_iterator>> Zip(const Iterative1& seq1, const Iterative2& seq2) {
    ZipIterator<typename Iterative1::const_iterator, typename Iterative2::const_iterator> begin_it(seq1.begin(), seq2.begin());
    ZipIterator<typename Iterative1::const_iterator, typename Iterative2::const_iterator> end_it(seq1.end(), seq2.end());

    return IteratorRange<ZipIterator<typename Iterative1::const_iterator, typename Iterative2::const_iterator>>(begin_it, end_it);

}