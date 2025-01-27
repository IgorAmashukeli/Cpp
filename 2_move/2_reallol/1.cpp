#include <iostream>
#include <vector>
#include <memory>
#include <sstream>
#include <string>
#include <random>
#include <time.h> 
#include <fstream>


std::string random_string(std::size_t length)
{
    const std::string CHARACTERS = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";

    std::random_device random_device;
    std::mt19937 generator(random_device());
    std::uniform_int_distribution<> distribution(0, CHARACTERS.size() - 1);

    std::string random_string;

    for (std::size_t i = 0; i < length; ++i)
    {
        random_string += CHARACTERS[distribution(generator)];
    }

    return random_string;
}


std::string generate_name(const std::vector<std::string>& str_vec) {
    if (rand() % 3 == 0) {
        return random_string(20);
    } else {
        return str_vec[rand() % str_vec.size()];
    }
}


double random_double() {
    std::random_device rd;
    std::mt19937 gen(rd());

    std::uniform_real_distribution<> dist(0.0, 1.0);
    return dist(gen);
}


int main() {

    std::ofstream file("output.txt"); 

    std::srand(time(0)); 

    std::vector<std::string> str_vec;

    for (size_t i = 0; i < 1000; ++i) {

        int cmd;

        if (str_vec.size() == 0) {
            cmd = 1;
        } else {
            cmd = rand() % 4 + 1;
        }
        

        file << cmd << "\n";

        if (cmd == 1) {

            std::string str = random_string(20);
            str_vec.push_back(str);
            file << str << "\n";

        } else if (cmd == 2) {

            file << generate_name(str_vec) << "\n";

            double rnd1 = random_double();
            double rnd2 = random_double();

            file << rnd1 << " " << rnd2 << "\n";

        } else if (cmd == 3) {

            file << generate_name(str_vec) << "\n";

        } else if (cmd == 4) {

            file << generate_name(str_vec) << "\n";
            file << generate_name(str_vec) << "\n";
        }
        
    }
}