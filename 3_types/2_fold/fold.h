#pragma once

// sum the two numbers
struct Sum {
    template<typename T>
    T operator() (const T& t1, const T& t2) {
        return t1 + t2;
    }
};

// multiply the two numbers
struct Prod {
    template<typename T>
    T operator() (const T& t1, const T& t2) {
        return t1 * t2;
    }
};

// concatenate two vectors
struct Concat {
    template<typename T>
    std::vector<T> operator() (const std::vector<T>& t1, const std::vector<T>& t2) {
        
        // if first is smaller, use the second as the main
        if (t1.size() < t2.size()) {
            std::vector<T> res(t2.begin(), t2.end());
            res.insert(res.end(), t1.begin(), t1.end());
            return res;

        // otherwise the second
        } else {
            std::vector<T> res(t1.begin(), t1.end());
            res.insert(res.end(), t2.begin(), t2.end());
            return res;
        }
    }
};

// fold operation
template <class Iterator, class T, class BinaryOp>
T Fold(Iterator first, Iterator last, T init, BinaryOp func) {
    

        
    // this whole way of writing is done so
    // because there can be non-default constructible types

    // if range is empty, return init
    if (first == last) {
        return init;
    } else {
        // here we can use func
        // to construct a value of type T
        T res = func(init, *first);
        for (auto it = first; it != last; ++it) {
            if (it != first) {
                res = func(res, *it);
            }
        }
        return res;
    }
}

class Length {
public:
    // save the pointer
    Length(int* length) : length_(length) {};

    // increment the length
    template<typename T>
    T operator() (const T& t1, const T& t2) {
        ++(*length_);
    
        // placeholder return: returns some T value
        return t2;
    }
private:
    int* length_;
};
