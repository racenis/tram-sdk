// TRAMWAY DRIFT AND DUNGEON EXPLORATION SIMULATOR 2022
// All rights reserved.

#ifndef TRAM_SDK_FRAMEWORK_UID_H
#define TRAM_SDK_FRAMEWORK_UID_H

#include <string>

namespace tram {

struct UID {
    UID() {}
    UID(const UID& value) { *this = value; }
    ~UID() {}
    
    UID(const std::string& value);
    UID(const char* value);
    
    UID& operator=(const UID& value) {
        key = value.key;
        return *this;
    }
    
    bool operator==(const UID& other) const {
        return key == other.key;
    }
    
    bool operator==(const char* other) const {
        return key == UID(other).key;
    }
    
    explicit operator bool() { return key; }
    
    operator std::string() const;
    operator char const*() const;
    
    static bool is_valid(const UID&);
    
    uint32_t key = 0;
};

typedef UID name_t;

}

#endif // TRAM_SDK_FRAMEWORK_UID_H