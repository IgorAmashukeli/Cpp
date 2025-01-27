#pragma once

#include <string>
#include <cstring>
#include <variant>

class StringView {

public:

    StringView(const std::string& s, size_t pos = 0, size_t size = std::string::npos) {
        pos_ = pos;
        if (pos > s.size()) {
            size_ = 0;
        } else {
            size_ = std::min(size, s.size() - pos);
        }
        s_ = &s;
    }

    StringView(const char* s)  {
        *this = StringView(s, strlen(s));
    }

    StringView(const char* s, size_t size) {
        pos_ = 0;
        size_ = size;
        s_ = s;
    }

    char operator[] (size_t i) const {
        if (std::holds_alternative<const char*>(s_)) {
            return std::get<const char*>(s_)[pos_ + i];
        } else {
            return (*std::get<const std::string*>(s_))[pos_ + i];
        }
    }

    size_t Size() const {
        return size_;
    }

private:

   std::variant<const char*, const std::string*> s_;
   size_t size_;
   size_t pos_;
};
