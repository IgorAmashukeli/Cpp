#include <iostream>
#include <vector>
#include <memory>
#include <sstream>


// Banner()
void Banner() {
    std::cout << "****************************************" << std::endl;
    std::cout << "*   Welcome to ReAL Location Service   *" << std::endl;
    std::cout << "****************************************" << std::endl;
    std::cout << "* 1) Register User                     *" << std::endl;
    std::cout << "* 2) Check In                          *" << std::endl;
    std::cout << "* 3) View feed                         *" << std::endl;
    std::cout << "* 4) Subscribe                         *" << std::endl;
    std::cout << "****************************************" << std::endl;
}

// get the line (size <= 80) from stdin
std::string LimitedGetline() {

    // maximum size is 80
    constexpr int kMaxSize = 80;

    // line
    std::string line;

    // while can read and size is correct read
    while (std::cin && line.size() < kMaxSize) {
        int symbol = std::cin.get();

        // line finished
        if (symbol == '\n') {
            break;
        }

        // update line
        line.push_back(symbol);
    }

    // return the answer
    return line;
}

// get the command
int GetCmd() {

    // read line
    std::string cmd_str = LimitedGetline();

    // extract int value
    std::stringstream stream(cmd_str);
    int cmd;
    stream >> cmd;

    // return it
    return cmd;
}

// location structure: longitude; latitude
struct Location {
    double longitude, latitude;
};

// User structure
struct User {

    // username string
    std::string username;

    // vector of posts
    std::vector<Location> posts;

    // vector of subscribers
    std::vector<User*> subscribers;

    // feed
    std::vector<std::pair<User*, Location*>> feed;
};

int main() {
    // number of users
    constexpr int kMaxUsers = 100;

    // vector of unique pointers of Users
    std::vector<std::unique_ptr<User>> users;

    // get user function
    auto get_user = [
        // catching variables by reference
        &]() 

        -> 

        // return type is User*
        User*
        
        {
        
        // asking for an username
        std::cout << "Enter username:";

        // reading username from stdin
        std::string username = LimitedGetline();

        // iterating over the vector of unique_ptr
        for (auto& user : users) {

            // found the first user with the same username
            if (user->username == username) {

                // get the User* value
                return user.get();
            }

        }

        // no user found
        std::cout << "User not found" << std::endl;

        // return nullptr
        return nullptr;
    };

    // 1000 iterations of work
    for (int i = 0; i < 1000; ++i) {

        // Banner output
        Banner();

        // reading a command
        // if command is not 1, 2, 3, 4
        // skip this and go to the next iteration of work
        switch (GetCmd()) {

            // command is 1
            // registration of a User
            case 1: {

                // there are already > 100 users
                // ? strange: shouldn't it be "user.size() >= kMaxUsers"
                if (users.size() > kMaxUsers) {
                    // log that it is a problem
                    std::cout << "Users limit reached" << std::endl;

                    // finish the branch
                    break;
                }

                // ask for the username
                std::cout << "Enter username:";

                // get the username
                std::string username = LimitedGetline();

                // push_back with the object, constructed with all the arguments
                // the argument is User*
                // the type is unique_ptr<User>
                // it calls move constructor (copy constructor is deleted)
                // it means, the memory is deallocated inside unique_ptr destructor
                // it means there is no memory leak

                users.emplace_back(

                    // allocating memory for a new User structure
                    // + initiallizing all the fields of the structure with default values
                    // returns User*

                    new User{}
                    
                );

                // take the last (push_backed) User object
                // take the posts object of User
                // create memory of 16 posts for the User
                users.back()->posts.reserve(16);

                // make the username - the one that was read in stdin
                users.back()->username = username;

                // write the information about the user
                std::cout << "Registered user " << username << std::endl;

                // finish the branch
                break;
            }

            // command is 2
            // check in for the user
            case 2: {

                // get the User*
                auto user = get_user();

                // break from this branch if there is no such user
                if (!user) {
                    break;
                }

                // ask for the location
                std::cout << "Enter your location:";
                Location location;

                // get the location
                std::string line = LimitedGetline();

                // getting the location of the user
                std::stringstream stream(line);
                stream >> location.longitude >> location.latitude;

                // emplacing back a location object (it copies the object)
                // ? can be "user->posts.emplace_back(std::move(location))"
                // not so important
                user->posts.emplace_back(location);

                // iterating over the subscribers of the user
                for (auto subscriber : user->subscribers) {

                    // the feed of the subscriber
                    subscriber->feed
                    
                    // constructing a pair from User* and Location*
                    // push_back of the pair to the vector
                    .emplace_back(
                        

                        // user pointer
                        user, 
                        
                        // location pointer
                        &
                        
                        // user posts
                        user->posts
                        
                        // get the last post
                        .back()
                    
                    
                    );
                }

                // log the fact that the user was checked in
                std::cout << "User " << user->username << " checked in" << std::endl;

                // finish the branch
                break;
            }

            // command is 3
            // view feed
            case 3: {

                // get the user
                auto user = get_user();

                // break from this branch if there is no such user
                if (!user) {
                    break;
                }

                // loging out that there will be a feed for the user
                std::cout << "Feed for user " << user->username << std::endl;

                // iterating over the user feed
                for (auto post : user->feed) {

                    // log username
                    std::cout << post.first->username << " is at " 
                    
                    // log longitude
                    << post.second->longitude << " "
                              
                    // log latitude
                    << post.second->latitude << std::endl;
                }

                // finish the branch
                break;
            }


            // Subscribe
            case 4: {

                // get the user
                auto user = get_user();

                // break from this branch if there is no such user
                if (!user) {
                    break;
                }

                // get the user, you want to subscribe
                auto subscribe_to = get_user();

                // break from this branch if there is no such user
                if (!subscribe_to) {
                    break;
                }
                
                // add the user as a subscriber to the "subscribe_to"
                subscribe_to->subscribers.push_back(user);

                // log the user username
                std::cout << "User " << user->username << " subscribed to "

                // log the subscriber username
                << subscribe_to->username << std::endl;

                // finish the branch
                break;
            }
        }
    }

    // everything is ok
    return 0;
}
