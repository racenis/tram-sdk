// TRAMWAY DRIFT AND DUNGEON EXPLORATION SIMULATOR 2022
// All rights reserved.

#include <framework/uid.h>

#include <cstring>

#include <templates/stackpool.h>
#include <templates/hashmap.h>

#include <murmur.h>

namespace tram {

static StackPool<char> string_pool("stringpool", 20000, {'n', 'o', 'n', 'e', '\0'});
static Hashmap<UID> string_list("stringlist", 2000, {{MurmurHash("none"), UID()}});

UID::UID (const std::string& value) {
    const char* str = value.c_str();
    *this = UID(str);
}

UID::UID (const char* value) {
    if (!value) {
        this->key = 0;
        return;
    }
    
    uint64_t hash = MurmurHash2(value, strlen(value) + 1);
    
    // check if value is already added
    UID existing = string_list.Find(hash);
    
    if (existing) {
        if (strcmp(existing, value) != 0) {
            std::cout << "UID collision " << existing << " with " << value << std::endl; 
            abort();
        }
        
        this->key = existing.key;
        return;
    }
    
    // special case. very important!
    if (strcmp("none", value) == 0) return;
    
    // else add to string table
    this->key = string_pool.size();
    
    char* new_value = string_pool.AddNew(strlen(value) + 1);
    strcpy(new_value, value);
    
    string_list.Insert(hash, *this);
}

UID::operator std::string() const {
    return string_pool.begin() + key;
}

UID::operator char const*() const {
    return string_pool.begin() + key;
}

bool UID::is_valid(const UID& value) {
    return value.key < string_pool.size();
}

}