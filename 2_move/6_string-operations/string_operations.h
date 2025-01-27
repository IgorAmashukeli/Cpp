#pragma once

#include <algorithm>
#include <cctype>
#include <string>
#include <string_view>
#include <vector>
#include <fstream>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cassert>
#include <numeric>


using STR1 = std::string;
using STR2 = std::string_view;

// checks that string starts with text
bool StartsWith(STR2 string, STR2 text);


// checks that string starts with text
bool EndsWith(STR2 string, STR2 text);




// removes prefix, if it exists
STR2 StripPrefix(STR2 string, STR2 prefix);

// removes suffix, if it exists
STR2 StripSuffix(STR2 string, STR2 suffix);



// takes the substring (if n > s.size, returns s)
STR2 ClippedSubstr(STR2 s, size_t pos, size_t n = STR2::npos);



// removes all the whitespaces from both ends
STR2 StripAsciiWhitespace(STR2);



// split strings, using delim
// only one allocation of memory is done
std::vector<STR2> StrSplit(STR2 text, STR2 delim);



// opens a file and reads n bytes, if not possible, returns empty string
STR1 ReadN(const STR1& filename, size_t n);


// adds '/' to the end of the file system path
STR1 AddSlash(STR2 path);



// removes '/' from the end of the file system path
STR2 RemoveSlash(STR2 path);


// get the directory name of the correct path to the file system directory
// incorrect paths are not allowed
STR2 Dirname(STR2 path);

// get the file name of the correct path to the file system directory
// incorrect paths are not allowed
STR2 Basename(STR2 path);



STR1 CollapseSlashes(STR2 path);


// join all the string with the delimiter
// only one allocation of memory is done
STR1 StrJoin(const std::vector<STR2>& strings, STR2 delimiter);






template<typename T>
bool inline constexpr is_cstring = std::is_pointer_v<T> || std::is_array_v<T>;


// returns the size of std::string to hold the integer type
template<typename T>
size_t GetIntSize(T&& t) {
    // type for U
    using U = typename std::remove_reference<T>::type;
    
    
    // if it is zero, return 1
    if (t == 0) {
        return 1;
    }
    // special byte for "-"
    size_t count = static_cast<size_t>(t < 0);
    

    // this is integer type : can copy
    U s = t;
    
    // find number of digits in the integer type
    while (s != 0) {
        s /= 10;
        ++count;
    }
    return count;
}


// template functions can be implented only in .h file

// template function to get size of the argument
template<typename T>
size_t GetSize(T&& t) {
    
    // removing references to get the type without it
    // (std::is_integral_v doesn't return true with references for integer types)
    using U = typename std::remove_reference<T>::type;
    
    // checks if the type was iteger
    if constexpr (std::is_integral_v<U>) {
        
        // returns the size of string to hold integer type
        return GetIntSize(std::forward<T>(t));
    
    // checks if the type was C string
    } else if constexpr (is_cstring<U>) {
        
        // return the length of the C string
        // we assume it is not nullptr
        return std::strlen(t);
        
    } else {
        
        // return the length of the std::string_view or std::string
        return t.size();
    }
}


template<typename T>
void PushBackInt(std::string& res, size_t& i, T&& t) {
    
    using U = typename std::remove_reference<T>::type;
    
    // if it is zero, push_back 0 and return
    if (t == 0) {
        res[i] = '0';
        ++i;
        return;
    }
    
    // if it is < 0, add minus at the start
    if (t < 0) {
        res[i] = '-';
    }
    
    // get the number of digits
    size_t size = GetIntSize(t);    

    // this is integer type : can copy
    U s = t;

    size_t j = i;
    
    while (s != 0) {
        res[j + size - 1] = '0' + (std::abs(static_cast<int>(s % 10)));
        
        s /= 10;
        
        --j;
    }

    i += size;
    
    
}


// function to push back arguments
template<typename T>
void PushBackArgs(std::string& res, size_t& i, T&& t) {
    using U = typename std::remove_reference<T>::type;
    
    
    // it is integer
    if constexpr (std::is_integral_v<U>) {
        
        // push back integer
        PushBackInt(res, i, std::forward<T>(t));
        
    } else if constexpr (is_cstring<U>) {
        
        
        
        // iterate untill null terminator to copy C string
        // we assume it is not nullptr
        const char* cur = t;
        while (*cur != '\0') {
            res[i] = *cur;
            ++cur;
            ++i;
        }
        
    } else {
        // push back the std::string/std::string_view
        std::copy(t.begin(), t.end(), res.begin() + i);
        i += t.size();
    }
}


// join all the arguments in their string representation
// <= 5 arguments are allowed
// argument types: 
// (unsinged) int, (unsigned) long, (unsigned) long long, std::string, std::string_view, const char*
// only one allocation of memory is done
template<typename... Args>
std::string StrCat(Args&&... args) {
    // there are no arguments
    if constexpr (sizeof...(args) == 0) {
        
        // nothing to do, return empty string
        return "";
        
    } else {
        
        // get the size of the returned string:
        // using binary left fold expression
        // = (...(0 + GetSize(args_1)) + ... + GetSize(args_n))
        // = GetSize(args_1) + ... GetSize(args_n)
        size_t size = (0 + ... + GetSize(std::forward<Args>(args)));
        
        
        // creating a result string and reserving enough memory for the string
        std::string res;
        res.resize(size);
        
        
        // do the push_backs on each argument
        size_t i = 0;

        (PushBackArgs(res, i, std::forward<Args>(args)), ...);
        
        // return the result string
        return res;
    }
}


