#pragma once

#include <cassert>
#include <vector>
#include <string>
#include <memory>


// Room class
class Room;

// Door class
class Door {
public:

    // checks, whether the door is open
    bool IsOpen() const {
        return open_;
    }

    // returns true if key opened door
    // otherwise false
    bool TryOpen(const std::string& key) {
        if (key_ == key) {
            open_ = true;
        }
        return open_;
    }

    // door must be open
    Room* GoThrough() {

        // checks that the door is open
        assert(open_);

        // if not
        
        return to_;
    }

    // creates door for the room without a key : it is opened
    explicit Door(Room* to) : open_(true), to_(to) {
    }

    // creates door for the room with a key : now it is closed
    Door(Room* to, const std::string& key) : open_(false), key_(key), to_(to) {
    }

    // door can't be constructed as copy or move
    Door(const Door&) = delete;
    Door(Door&&) = delete;

    // door can't be copied or moved
    Door& operator=(const Door&) = delete;
    Door& operator=(Door&&) = delete;

private:

    // whether door is opened
    bool open_;

    // key to open the door
    std::string key_;

    // pointer to the room, which it opens
    Room* to_;
};


// room class
class Room {
public:

    // checks, whether the room is final
    bool IsFinal() const {
        return is_final_;
    }

    // finds the number of keys, laying down on the floor of the room
    size_t NumKeys() const {
        return keys_.size();
    }

    // returns the key of the door of the room
    std::string GetKey(size_t key_index) const {
        return keys_[key_index];
    }

    // fins the number of doors in the room
    size_t NumDoors() const {
        return doors_.size();
    }

    // returns one of the door in the room
    Door* GetDoor(size_t door_index) {
        return doors_[door_index].get();
    }

    // creates room
    explicit Room(bool is_final) : is_final_(is_final) {
    }

    // adds open door
    void AddDoor(Room* to) {

        // 1) allocates memory for Door type
        // 2) constructs open door for the "to" (destination) room
        // 3) constructs std::unque_ptr object with it
            // no memory leaks: the memory is deleted inside unique_ptr
        // 4) push_backs std::unique_ptr object
        // (adds smart pointer to the "to" door to the array of doors of this room)
        // 5) may be reallocation: previous unique_ptrs will be moved
        doors_.emplace_back(new Door(to));

        // 1) allocates memory for Door type
        // 2) constructs open Door for this room
        // 3) constructs std::unque_ptr object with it
            // no memory leaks: the memory is deleted inside unique_ptr
        // 4) push_backs std::unique_ptr object
        // (adds smart pointer to this door to the array of doors of "to" room)
        // 5) may be reallocation: previous unique_ptrs will be moved
        to->doors_.emplace_back(new Door(this));
    }

    // does almost the same as the previous question
    // but creates closed doors
    // (both sides with the same keys)
    void AddDoorWithKey(Room* to, const std::string& key) {
        
        doors_.emplace_back(new Door(to, key));
        to->doors_.emplace_back(new Door(this, key));
    }

    // adds key to the array of keys on the floor
    void AddKey(const std::string& key) {
        keys_.push_back(key);
    }

    // room can't be constructed as copy or move
    Room(const Room&) = delete;
    Room(Room&&) = delete;

    // room can't be copied or moved
    Room& operator=(const Room&) = delete;
    Room& operator=(Room&&) = delete;

private:
    // whether the room is final
    bool is_final_;

    // array of doors in the room
    std::vector<std::unique_ptr<Door>> doors_;

    // array of keys, laying on the floor
    std::vector<std::string> keys_;
};
