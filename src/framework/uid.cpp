// Tramway Drifting and Dungeon Exploration Simulator SDK Runtime

#include <framework/uid.h>

#include <cstring>

#include <templates/stackpool.h>
#include <templates/hashmap.h>

#include <config.h>

#include <murmur.h>

/**
 * @struct tram::UID framework/uid.h
 * 
 * Interned string type.
 * 
 * Should intern strings in O(1) as long as the total number of interned strings
 * is lower than UID_LIMIT. 
 * In either case, interned string comparison will always be O(1).
 * If you are converting a lot of strings, it is best to do so early, because
 * the internment process, altough nearly constant time, does, in fact, take
 * some time.
 * 
 * @see https://racenis.github.io/tram-sdk/documentation/framework/uid.html
 */
 
 /**
 * @typedef UID name_t
 * Alias for UID.
 */

namespace tram {

static StackPool<char> string_pool("Name pool", UID_CHARACTER_LIMIT, {'n', 'o', 'n', 'e', '\0'});
static Hashmap<UID> string_list("Name list", UID_LIMIT, {{MurmurHash("none"), UID()}});

UID::UID(const std::string& value) {
    const char* str = value.c_str();
    *this = UID(str);
}

// TODO: make this THREAD SAFE!!
UID::UID(const char* value) {
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