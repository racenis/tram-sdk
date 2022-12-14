// TRAMWAY DRIFT AND DUNGEON EXPLORATION SIMULATOR 2022
// All rights reserved.

#ifndef FRAMEWORK_UID_H
#define FRAMEWORK_UID_H

#include <string>

namespace Core {
    struct UID {
        UID() {}
        UID(const std::string& value);
        UID(const char*& value);
        UID(const uint64_t& value);
        ~UID() { }
        UID(const UID& value) { *this = value; }
        UID& operator=(const UID& value) {
            key = value.key;
            return *this;
        }
        
        bool operator==(const UID& other) const {
            return key == other.key;
        }
        
        bool operator==(const uint64_t& other) const {
            return key == other;
        }
        
        explicit operator bool() { return key; }
        
        operator std::string() const;
        operator char const*() const;
        
        uint64_t key = 0;
    };
    
    typedef UID name_t;
}

#endif // FRAMEWORK_UID_H