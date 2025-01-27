#pragma once

#include <vector>

template <class Functor>
class ReverseBinaryFunctor {
public:
    ReverseBinaryFunctor(const Functor& functor) :functor_(functor) {};

    template<typename... Args>
    bool operator() (Args&&... args) {

        return !functor_(std::forward<Args>(args)...);
    }
private:
    Functor functor_;
};

template <class Functor>
class ReverseUnaryFunctor {
public:
    ReverseUnaryFunctor(const Functor& functor) :functor_(functor) {};

    template<typename... Args>
    bool operator() (Args&&... args) {

        return !functor_(std::forward<Args>(args)...);
    }

private:
    Functor functor_;
};

template <class Functor>
ReverseUnaryFunctor<Functor> MakeReverseUnaryFunctor(Functor functor) {
    return ReverseUnaryFunctor<Functor>(functor);
}

template <class Functor>
ReverseBinaryFunctor<Functor> MakeReverseBinaryFunctor(Functor functor) {
    return ReverseBinaryFunctor<Functor>(functor);
}
