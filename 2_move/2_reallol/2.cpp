#include <iostream>
#include <vector>
#include <memory>
#include <sstream>
#include <string>
#include <random>
#include <time.h> 
#include <fstream>
#include <cstdlib>
#include <iostream>


int main() {
    int return_value = 0;

    std::string input = "output.txt";
    std::string server = "./server";
    std::string test_gen = "test";

    while (return_value == 0) {
        system(test_gen.c_str());

        std::string command = server + " < " + input;
        
        return_value = system(command.c_str());
    }

    std::cout << "Program stopped with return value: " << return_value << std::endl;

    return 0;
}


