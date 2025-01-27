#pragma once

#include "dungeon.h"
#include <unordered_map>
#include <unordered_set>




Room* FindFinalRoom(Room* starting_room) {
    // no dungeon -> no escape
    if (starting_room == nullptr) {
        return nullptr;
    }

    // all closed doors
    std::unordered_set<Door*> closed_doors;

    // all opened doors
    std::unordered_set<Door*> opened_doors;

    // all found keys
    std::unordered_set<std::string> keys;

    // all visited rooms
    std::unordered_set<Room*> visited = {starting_room};
    
    // starting room
    Room* cur = starting_room;

    while (true) {
        // it current room is final
        if (cur->IsFinal()) {
            return cur;
        }
        // find the number of doors in cur room
        size_t door_count = cur->NumDoors();

        // add doors
        for (size_t i = 0; i < door_count; ++i) {
            if (cur->GetDoor(i)->IsOpen()) {
                opened_doors.insert(cur->GetDoor(i));
            } else {
                closed_doors.insert(cur->GetDoor(i));
            }
            
        }

        // get the number of keys in cur room
        size_t keys_count = cur->NumKeys();

        // iterate over the keys on the floor of the room
        for (size_t i = 0; i < keys_count; ++i) {
            keys.insert(cur->GetKey(i));
        }
        
        for (const auto& key : keys) {

        // try to open all the closed doors
            for (auto it = closed_doors.begin(); it != closed_doors.end();) {
                // get the door
                Door* cl_door = *it;

                // try to open it with the key
                bool result = cl_door->TryOpen(key);
                // found a key for the door
                if (result) {

                    // make the door
                    opened_doors.insert(cl_door);

                    // erase the door from closed
                    it = closed_doors.erase(it);
                } else {

                    // this door remains closed, move to the next
                    ++it;
                }
            }

        }

        Door* to_visit = nullptr;

        

        // go through all open doors
        for (auto it = opened_doors.begin(); it != opened_doors.end();) {

            // get the door
            Door* door = *it;

            // go through the door
            Room* room = door->GoThrough();

            // if it is a new room
            if (visited.find(room) == visited.end()) {

                // now it is new current room
                cur = room;

                // make it visited
                visited.insert(cur);

                // remove the door
                // now there is no need to go through this door
                opened_doors.erase(it);

                // remember the door
                to_visit = door;

                // stop checking open doors
                break;

            } // otherwise: continue

            // this door lead to a visited room
            // also no need to go throug this door no more
            it = opened_doors.erase(it);
        }

        // all open doors lead to visited rooms
        // this means that with all the keys
        // found in the rooms we can visit
        // there are no new ones we can visit
        // we covered all the starting connected component
        // no final room found
        if (to_visit == nullptr) {
            return nullptr;
        } 

    }
}

