#include "string_operations.h"
#include <string>
#include <fcntl.h>
#include <unistd.h>


// 1) for this function, there is no need for std::string
// (there is no allocation needed)
// 2) std::string_view can be passed by value
// (it is not bigger than 2 or 3 byte words)
// all the copy constructors and copy assignment called are trivial
// because std::string_view is trivially copyable type
bool StartsWith(std::string_view string, std::string_view text) {

    // text.size() > string.size()
    if (text.size() > string.size()) {
        return false;
    }

    // checks that string is greater or equal to text
    return std::equal(text.begin(), text.end(), string.begin(), string.begin() + text.size());
}

// 1) for this function, there is no need for std::string
// (there is no allocation needed)
// 2) std::string_view can be passed by value
// (it is not bigger than 2 or 3 byte words)
// all the copy constructors and copy assignment called are trivial
// because std::string_view is trivially copyable type
bool EndsWith(std::string_view string, std::string_view text) {
    
    // text.size() > string.size()
    if (text.size() > string.size()) {
        return false;
    }

    // checks that string is greater or equal to text
    return std::equal(text.begin(), text.end(), string.begin() + (string.size() - text.size()), string.end());
}


// 1) for this function there is no need for std::string
// (there is no allocation needed)
// 2) std::string_view can be passed by value
// (it is not bigger than 2 or 3 bytw words)
// 3) std::string_view can be returned by value
// all the copy constructors and copy assignment called are trivial
// because std::string_view is trivially copyable type
std::string_view StripPrefix(std::string_view string, std::string_view prefix) {

    // string doesn't start with the prefix
    if (!StartsWith(string, prefix)) {
        return string;
    }

    string.remove_prefix(prefix.size());

    return string;
}


// 1) for this function there is no need for std::string
// (there is no allocation needed)
// 2) std::string_view can be passed by value
// (it is not bigger than 2 or 3 bytw words)
// 3) std::string_view can be returned by value
// all the copy constructors and copy assignment called are trivial
// because std::string_view is trivially copyable type
std::string_view StripSuffix(std::string_view string, std::string_view suffix) {

    // string doesn't end with the suffix
    if(!EndsWith(string, suffix)) {
        return string;
    }

    string.remove_suffix(suffix.size());

    return string;
}



// 1) for this function there is no need for std::string
// (there is no allocation needed)
// 2) std::string_view can be passed by value
// (it is not bigger than 2 or 3 bytw words)
// 3) std::string_view can be returned by value
// all the copy constructors and copy assignment called are trivial
// because std::string_view is trivially copyable type

// ? strange interface in the requirements:
// if n > s.size() -> return string;
// maybe if pos + n > s.size() -> return string ??
std::string_view ClippedSubstr(std::string_view string, size_t pos, size_t n) {

    // if n > string.size() => return string
    if (n > string.size()) {
        return string;
    }

    return string.substr(pos, n);

}


// striping whitespaces from front and back

// 1) for this function there is no need for std::string
// (there is no allocation needed)
// 2) std::string_view can be passed by value
// (it is not bigger than 2 or 3 byte words)
// 3) std::string_view can be returned by value
// all the copy constructors and copy assignment called are trivial
// because std::string_view is trivially copyable type
std::string_view StripAsciiWhitespace(std::string_view string) {

    string.remove_prefix(std::find_if_not(string.begin(), string.end(), 
    [](char x) {return std::isspace(x);}) - string.begin());

    string.remove_suffix(std::find_if_not(string.rbegin(), string.rend(), 
    [](char x) {return std::isspace(x);}) - string.rbegin());

    return string;
}


size_t CountDelim(std::string_view text, std::string_view delim) {
    
    // number of delimetrs
    size_t count = 0;

    // position
    size_t pos = 0;

    // find the position
    while (pos < text.size()) {
        // first position starts from the start of the string
        if (count == 0) {
            pos = text.find(delim, 0);
        
        // next position starts from the next part of the string
        } else {
            pos = text.find(delim, pos + 1);
        }

        // increment the number of the delimiters
        ++count;
    }
    
    // this could never be < 1 (because pos changes at the first iteration
    // and there will be at least one iteration of this loop, because 0 = pos < text.size())
    // because text.size() > 0
    // because text is not empty
    assert (count >= 1);


    return count - 1;
}



// 1) for this function there is no need for std::string
// (there is no allocation needed)
// 2) std::string_view can be passed by value
// (it is not bigger than 2 or 3 byte words)e


// if copy elision (NRVO) works => there is no copy/move constructor called
// if NRVO doesn't work => std::vector is movable => 
// only move constructors/move assignment operator
//  will be called => the buffer is transferred from std::vector first owner to the next owner

std::vector<std::string_view> StrSplit(std::string_view text, std::string_view delim) {
    // if delim is empty or text is empty, nothing to do
    if (delim.empty() || text.empty()) {
        return {text};
    }
    
    // count the number of delimiters

    // the number of splitted parts should be one bigger than number of delimiters
    size_t count = CountDelim(text, delim) + 1;
    
    if (count == 1) {
        return {text};
    }
    
    // creating vector of the answer
    
    std::vector<std::string_view> answer;
    answer.resize(count);

    size_t prev_pos, pos = 0;
    
    // fill the splitted vector
    for (size_t i = 0; i < answer.size(); ++i) {

        // the string_view views the same text string
        answer[i] = text;
        

        // if it is the first delimiter, find the first delimiter from the start of the string 
        if (i == 0) {
            prev_pos = pos;
            pos = text.find(delim, 0);
        

        // else find the the first delimiter after the next part of the string
        } else {
            prev_pos = pos;
            pos = text.find(delim, pos + 1);
        }

        
        // if it is not the first splitted, move start to the previous pos + delim
        // (which is right after the previous delimiter)
        // (which is the start of the current string)
        if (i != 0) {
            answer[i].remove_prefix(prev_pos + delim.size());
        }

    
        // if it is not the last splitted
        // pos - is the current new delimiter position
        // pos - 1 is the last position of the string before the delimiter
        // pos is the size of the string before the delimiter
        // text.size() - pos it the suffix that should be removed
        if (i + 1 != count) {
            answer[i].remove_suffix(text.size() - pos);
        }
    }

    return answer;
}


// need to acceess string.c_str to take the filename with '\0'
// (otherwise, linux sys call will fail)
// therefore, takes const std::string& filename as an argument

// need to acces string.data to write the data from the file
// therefore returns std::string as a type of the result

std::string ReadN(const std::string& filename, size_t n) {
    // n < 0 shouldn't be passed as an argument
    // if n == 0 => nothing to do => return ""
    if (n == 0) {
        return "";
    }

    // opens a file
    int fd = open(filename.c_str(), O_RDONLY);

    // can't open file -> return empty string
    if (fd < 0) {
        return "";
    }

    // string result
    std::string res(n, '0');

    ssize_t ret_code = read(fd, static_cast<void*>(&res[0]), n);


    
    // something went wrong with reading -> return empty string
    if (static_cast<size_t>(ret_code) != n) {
        return "";
    }

    // close the file
    close(fd);
    
    // return result
    // hope: there will be copy elision or move constructor called
    return res;
}






// need to return std::string, because the allocated memory couldn't be destroyed in the function
// need to take std::string_view path to be able to create std::string from it fastly
std::string AddSlash(std::string_view path) {


    // no need to add slash
    if (path.size() > 0 && path.back() == '/') {
        return std::string{path};
    }

    // there is need for allocation
    std::string res(path.size() + 1, '/');
    std::copy(path.begin(), path.end(), res.begin());

    
    return res;
    
}


// doesn't modify the memory of the string
// can pass by std::string_view

// 1) for this function there is no need for std::string
// (there is no allocation needed)
// 2) std::string_view can be passed by value
// (it is not bigger than 2 or 3 byte words)
// 3) std::string_view can be returned by value
// all the copy constructors and copy assignment called are trivial
// because std::string_view is trivially copyable type
std::string_view RemoveSlash(std::string_view path) {

    // if '/' (path.size() == 1) -> don't remove
    // if last element is '/' -> remove
    if (path.size() > 1 && path.back() == '/') {
        path.remove_suffix(1);
    }

    return path;
}




// 1) for this function there is no need for std::string
// (there is no allocation needed)
// 2) std::string_view can be passed by value
// (it is not bigger than 2 or 3 byte words)
// 3) std::string_view can be returned by value
// all the copy constructors and copy assignment called are trivial
// because std::string_view is trivially copyable type
std::string_view Dirname(std::string_view path) {

    // if path empty -> it is a root path -> return '/'
    if (path.empty()) {
        return "/";
    }

    // count - number of '/'
    size_t count = std::count_if(path.begin(), path.end(), [](char x){return (x == '/');});

    // path can't have no '/' -> it is a correct path
    assert(count >= 1);

    // if it is a root directory => return '/'
    if (count == 1) {

        return "/";
    }
    

    // if count > 1 => return the directory name of the file path
    // remove filename part of the string
    else {

       
        size_t lst_sl = path.rfind('/');
        
        // m is the position of the last '/'
        // 0 - m-1 is the positions of the dirname
        // m is the size of the dirnmae string
        // what to remove is path.size() - m
        path.remove_suffix(path.size() - lst_sl);


        return path;
    }
}


// 1) for this function there is no need for std::string
// (there is no allocation needed)
// 2) std::string_view can be passed by value
// (it is not bigger than 2 or 3 byte words)
// 3) std::string_view can be returned by value
// all the copy constructors and copy assignment called are trivial
// because std::string_view is trivially copyable type
std::string_view Basename(std::string_view path) {

    // if path is empty -> it is strange, return ""
    if (path.empty()) {
        return "";
    }

    // if path is actually '/' -> return ""
    if (path.size() == 1) {
        return "";
    }

    // remove slahse at the end (except "/") (because there can be at the end)
    path = RemoveSlash(path);


    // it should have at least one slash

    // m is the position of the last '/'
    // 0 - m-1 are the positions of the string before this '/'
    // the size of the substring from the beginning till the last '/' is m + 1

    size_t lst_sl = path.rfind('/');


    path.remove_prefix(lst_sl + 1);
    
    return path;
    
}





// the string will be changed
// there will be allocations
// there is a need to return std::string


// function that tries to collapse slashes without memory allocation
std::string_view TryNoAllocCollapse(std::string_view path, bool& res_flag) {
    // path is empty -> nothing to do
    if (path.empty()) {
        res_flag = true;
        return path;
    }

    // find the first non '/' character
    size_t f_nsl = path.find_first_not_of('/');

    // there is no not '/' characters -> all characters are '/'
    if (f_nsl == std::string_view::npos) {

        res_flag = true;

        // remove all last characters, except the first
        path.remove_suffix(path.size() - 1);

        return path;
    }


    size_t l_nsl = path.find_last_not_of('/');

    // if there is no first non '/' character => there is no last '/' character
    assert(l_nsl != std::string_view::npos);





    // there are no '/' in the range between the first non-'/' and last non-'/'
    if (std::find(path.begin() + f_nsl, path.begin() + l_nsl + 1, '/') == (path.begin() + l_nsl + 1)) {
        res_flag = true;
        
        // if it is not the first character, it means, there are slashes before
        // we need to include one of them
        if (f_nsl > 0) {
            --f_nsl;
        }
        
        
        // if it is not the last character, it means, there are slashes after
        // we need to include one of them
        if (l_nsl < path.size()) {
            ++l_nsl;
        }
        
        return path.substr(f_nsl, l_nsl - f_nsl + 1);
    }

    // it was unsuccesfull : there are '/' in the middle, can't remove prefix and suffix
    // it requires allocation
    res_flag = false;

    return path;
}


// if view has form of '///...///', then it doesn't need allocation
// if there are extra slashes between the middle elements
// we need allocation
// therefore, we return std::string

// instead, we could do remove_if of the extra slashes
// and then, we can remove_suffix (remove_if will return new iterator it that should
// serve as the .end() iterator, based on it we calculate suffix size as path.end() - it)
// however, this will modify the input data
// it is forbidden
// (in the test, it checks, that in that situation, there is at least one memory allocation)
std::string CollapseSlashes(std::string_view path) {

    // try no allocation collapse
    bool res_flag;
    path = TryNoAllocCollapse(path, res_flag);


    // if succesfull, return it
    if (res_flag) {
        return std::string(path);
    }
    
    


    // if not succesfull, find the size of the new string
    size_t i = 0;
    size_t count = std::count_if(path.begin(), path.end(), [&i, &path](char x){

        bool res = (x != '/') || (i == 0) || (path[i - 1] != '/');

        ++i;
        
        return res;
        
    });
    


    // create result string
    std::string res(count, '0');

    // fill all the values, skipping double '/'
    i = 0;
    std::for_each(res.begin(), res.end(), [&i, &path](char& x){
        while ((path[i] == '/') && i != 0 && path[i - 1] == '/') {
            ++i;
        }
        x = path[i];
        ++i;
        
    });
    


    return res;
}


// function is doing allocation for the result
// therefore, there is need to return string
std::string StrJoin(const std::vector<std::string_view>& strings, std::string_view delimiter) {
    if (strings.empty()) {
        return "";
    }

    size_t size =
    // count the sum of all sizes of strings
    std::accumulate(
        strings.begin(),
        strings.end(),
        0,
        [](const size_t& init, std::string_view cur) 
        {
            return init + cur.size();

        }) 
        
        // add delimiters
        + ((strings.size() - 1) * delimiter.size());

    // result string
    std::string res;

    // we are doing reserve instead of resize to be able to do back inserting
    res.reserve(size);

    size_t i = 0;
    std::for_each(strings.begin(), strings.end(), [&i, &res, &strings, delimiter](std::string_view cur){
        std::copy(cur.begin(), cur.end(), std::back_inserter(res));
        if (i + 1 != strings.size()) {
            std::copy(delimiter.begin(), delimiter.end(), std::back_inserter(res));
        }
        ++i;
    });


    return res;
}

