#include <iostream>
#include <vector>
#include <memory>
#include <sstream>


// get limited line
std::string LimitedGetline() {

    // constant compiler int
    constexpr int kMaxSize = 80;

    // string
    std::string line;

    // reading from the stdin
    while (std::cin && line.size() < kMaxSize) {

        // getting the symbol
        int symbol = std::cin.get();

        if (symbol == '\n') {
            break;
        }
        line.push_back(symbol);
    }

    // return the line
    return line;
}

// This class is following rule of 5.
// http://en.cppreference.com/w/cpp/language/rule_of_three


// SlowNumber class
class SlowNumber {
public:

    // constructor
    SlowNumber(int value)
    
        // creates int* in dynamic memory
     : value_(new int(value)) {


    }


    // move constructor
    SlowNumber(SlowNumber&& other) 
    
     // initialize the field with the other value
    : value_(other.value_) {

        // makes other value equal to nullptr
        other.value_ = nullptr;
    }


    // copy constructor
    SlowNumber(const SlowNumber& other) : 
    
    // create new memory, initilialize it with the value, stored in other field
    value_(new int(*other.value_)) {
    }


    // move assignment operator
    SlowNumber& operator=(SlowNumber&& other) {
        
        // if the value_ is not nullptr, deletes it
        
        // doesn't work with self-assignment and nullptr
        // (if value_ = other.value_ is nullptr, it will crash on deleting)
        // (if value_ = other.value_ is not nullptr, it will delete the value_)
        if (value_) {
            delete value_;
        }


        // changing value to other.value
        
        // if it is self-assignment and other.value_ is not nullptr, it does nothing
        value_ = other.value_;

        // making other.value_ equal to nullptr
        other.value_ = nullptr;

        // if there was some memory in value_, it was deleted
        // it is incorrect
        return *this;
    } // self assignment is working incorrectly


    // copy assignment operator
    SlowNumber& operator=(const SlowNumber& other) {

        // deletes the current value_ and makes it nullptr

        // if it is self-assignment and value_ is nullptr, it will crash
        // if it is self-assignment and value_ is not nullptr, we delete other.value_
        if (value_) {
            delete value_;
            value_ = nullptr;
        }

        // if it is not self-assignment and other.value_ is nullptr, we have correct value_
        // if it is not self-assignment and other.value_ is not nullptr, we are creating new memory
        // and assigning it to the correct value
        // ? why not to make value_* = other.value and not delete the memory in the case of other.value_ != nullptr

        // if it is self-assignment and other.value_ was not nullptr, we deleted it, it is not correct
        if (other.value_) {
            value_ = new int(*other.value_);
        }
        return *this;
    }

    // delete the memory
    ~SlowNumber() {
        if (value_) {
            delete value_;
        }
    }

    // get the value
    int Value() {
        return *value_;
    }

private:

    // the memory itself
    int* value_ = nullptr;
};

int main() {

    // vector of shared ptrs of SlowNumbers
    std::vector<std::shared_ptr<SlowNumber>> stack;

    for (int i = 0; i < 1000; ++i) {

        // get command
        auto cmd = LimitedGetline();

        // the command is "push"
        if (cmd == "push") {

            // get the value
            int value = 0;
            std::cin >> value;
            std::cin.get();  // eat up newline







            // constructs an object of shared_ptr and push_backs it, it calls move constructor
            // move constructor of shared_ptr acquires the same SlowNumber*
            // and has the same number of references, as the temporary object
            // the temporary object is destroyed
            // the SlowNumber object is not destroyed
            // (because it is not the last object, owning the pointer)
            

            // may call reallocation -> the pointers to the shared_ptr can be invalidated
            // if reallocation is invoked, it means that
            // there are new copies of shared_ptr that are created
            // in the process of reallocation, the previous copies are destroyed, when the memory is freed
            // destruction of this copies doesn't destroy the SlowNumber object

            stack.emplace_back(
                
                // creates new memory (int*) for value
                // creates shared_ptr, which owns SlowNumber*
                std::make_shared<SlowNumber>(value)
                
            );

        } else if (cmd == "pop") {

            // if stack is not empty
            if (!stack.empty()) {

                // if stack.size() was > 1
                // it destroys the shared_ptr, which doesn't change the SlowNumber object

                // if stack.size() was 1
                // it destroys the last shared_ptr, which deletes the SlowNumber object
                // it calls destructor
                // destructor deletes the memory
                stack.pop_back();


            } else {

                // nothing happens
                std::cout << "error: stack is empty" << std::endl;
            }
        } else if (cmd == "print") {

            // prints the values of the objects
            for (auto& v : stack) {
                std::cout << v->Value() << std::endl;
            }
        } else if (cmd == "swap") {


            size_t i = 0;
            size_t j = 0;
            std::cin >> i >> j;
            std::cin.get();  // eat up newline

            // nothing happens
            if (i < 0 || i >= stack.size() || j < 0 || j >= stack.size()) {
                std::cout << "error: invalid index" << std::endl;
                continue;
            }

            // SlowNumber t = *stack[i];
            // *stack[i] = *stack[j]
            // *stack[j] = t
            // it will call move assignment operator of SlowNumber
            // if i = j, it will call move assignment operator of the same object
            // self-assignment is written incorrectly for the SlowNumber
            // but let's look more closely:

            // t = *stack[i] - move constructor is called
            // t now holds the value
            // *stack[i] = *stack[i] - this will delete the value
            // *stack[i] = t
            // now it *stack[i] holds the correct value again
            std::swap(*(stack[i]), *(stack[j]));


        } else if (cmd == "dup") {

            // nothing happens
            if (stack.empty()) {
                std::cout << "error: stack is empty" << std::endl;
                continue;
            }

            // stack.back is the link to the last shared_ptr 
            // pushing back this shared_ptr
            // it is calling move constructor for shared_ptr
            // can do reallocation and invalidate pointers to shared_ptr

            // reallocation does copy to the shared_ptr
            // this copy will work, it will just have the same reference to SlowNumber*
            stack.push_back(stack.back());

        } else if (cmd == "pop_carry") {

            // nothing happens
            if (stack.size() < 2) {
                std::cout << "error: stack size must be greater than 2" << std::endl;
                continue;
            }

            // move the last value to the stack.size() - 2 value
            // calls std::move on the SlowNumber
            // will fail, if the SlowNumber, storing in the back shared_ptr
            // is the same as the one, which is before it
            // this will call move operator with self-assignment
            // here, it will fail, if stack.back is the same as the stack.size() - 2
            *(stack[stack.size() - 2]) = std::move(*stack.back());

            // destroys the element
            stack.pop_back();

        
        
        // wrong commands
        } else if (cmd == "") {
            break;
        } else {
            std::cout << "unknown cmd" << std::endl;
        }
    }

    return 0;
}
