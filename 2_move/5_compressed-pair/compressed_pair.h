
#pragma once

#include <type_traits>
#include <utility>


template <class V>
inline constexpr bool kIsCompressed = std::is_empty_v<V> && !std::is_final_v<V>;

template <class U, class V>
inline constexpr bool kSizeTwo = kIsCompressed<U> && kIsCompressed<V> && (std::is_base_of_v<U, V> || std::is_base_of_v<V, U>);

template <class U, class V>
inline constexpr bool kSizeOne = !kSizeTwo<U, V> && kIsCompressed<U> && kIsCompressed<V>;

template <class U, class V>
inline constexpr bool kSizeFirst = !kSizeTwo<U, V> && !kSizeOne<U, V> && kIsCompressed<V> && !std::is_base_of_v<V, U>;

template <class U, class V>
inline constexpr bool kSizeFirstTwo = kSizeFirst<U, V> || kSizeTwo<U, V>;

template <class U, class V>
inline constexpr bool kSizeSecond = !kSizeTwo<U, V> && !kSizeOne<U, V> && !kSizeFirst<U, V> && kIsCompressed<U> && !std::is_base_of_v<U, V>;

template <class U, class V>
inline constexpr bool kSizePair =  !kSizeTwo<U, V> && !kSizeOne<U, V> && !kSizeFirst<U, V> && !kSizeSecond<U, V>;


// generic template
// just a list of method declarations
template <typename F, typename S, typename Enable = void>
class CompressedPair {
public:
    CompressedPair();
    template<typename Ftemp, typename Stemp>
    CompressedPair(Ftemp&& f, Stemp&& s);
    const F& GetFirst() const;
    const S& GetSecond() const;
    F& GetFirst();
    S& GetSecond();

private:
    
};


// specification, when check size should return sizeof std::pair<F, S>
// we can implement it as a simple std::pair<F, S>
template <typename F, typename S>
class CompressedPair<F, S, typename std::enable_if<kSizePair<F, S>>::type> {
public:


    // default constructor
    CompressedPair() = default;
    

    // constructor for lvalue and rvalue first and second values
    // it will use std::forward to correctly call the corresponding constructor
    // constructor doesn't work without templates
    template<typename Ftemp, typename Stemp>
    CompressedPair(Ftemp&& f, Stemp&& s) : f_{std::forward<Ftemp>(f)}, s_{std::forward<Stemp>(s)} {}

    // return const F& and S& references
    const F& GetFirst() const {
        return f_;
    }


    const S& GetSecond() const {
        return s_;
    }

    // return F& and S& references
    F& GetFirst() {
        return f_;
    }


    S& GetSecond() {
        return s_;
    }

private:
    F f_{};
    S s_{};
};


// specification for the case, when size should be the sizeof(F) or == 2
template <typename F, typename S>
class CompressedPair<F, S, typename std::enable_if<kSizeFirstTwo<F, S>>::type> : public S {
public:
    // default constructor
    CompressedPair() = default;

    template<typename Ftemp, typename Stemp>
    CompressedPair(Ftemp&& f, [[maybe_unused]] Stemp&& s) : f_{std::forward<Ftemp>(f)} {}

    const F& GetFirst() const {
        return f_;
    }
    const S& GetSecond() const {
        return *this;
    }


    F& GetFirst() {
        return f_;
    }
    S& GetSecond() {
        return *this;
    }


private:
    F f_{};
};


// specification for the case, when size should be the sizeof(S)
template <typename F, typename S>
class CompressedPair<F, S, typename std::enable_if<kSizeSecond<F, S>>::type> : public F {
public:

    // default constructor
    CompressedPair() = default;


    template<typename Ftemp, typename Stemp>
    CompressedPair([[maybe_unused]] Ftemp&& f, Stemp&& s) : s_{std::forward<Stemp>(s)} {}

    const F& GetFirst() const {
        return *this;
    }
    const S& GetSecond() const {
        return s_;
    }

    F& GetFirst() {
        return *this;
    }
    S& GetSecond() {
        return s_;
    }

private:
    S s_{};
};


// specification for the case, when size should be sizeof == 1
template <typename F, typename S>
class CompressedPair<F, S, typename std::enable_if<kSizeOne<F, S>>::type> : public F, public S {
public:

    // default constructor
    CompressedPair() = default;


    template<typename Ftemp, typename Stemp>
    CompressedPair([[maybe_unused]] Ftemp&& f,  [[maybe_unused]] Stemp&& s) {}

    const F& GetFirst() const {
        return *this;
    }
    const S& GetSecond()  const{
        return *this;
    }

    F& GetFirst() {
        return *this;
    }
    S& GetSecond() {
        return *this;
    }

private:
};

