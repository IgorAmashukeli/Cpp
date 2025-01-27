#include "harakiri.h"

// Your code goes here


AwesomeCallback::AwesomeCallback(const std::string& str) {
    str_ = str;
}


std::string AwesomeCallback::operator()() const&& {
    std::string awesome_res = std::move(str_) + "awesomeness";

    delete this;

    return awesome_res;
}

