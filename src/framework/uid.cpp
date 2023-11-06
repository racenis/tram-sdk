// TRAMWAY DRIFT AND DUNGEON EXPLORATION SIMULATOR 2022
// All rights reserved.

#include <framework/uid.h>

#include <cstring>
#include <templates/stackpool.h>

#include <unordered_map>

namespace tram {

static StackPool<char> string_pool ("stringpool", 10000, {'n', 'o', 'n', 'e', '\0'});
static std::unordered_map<std::string, uint64_t> string_list = {{"none", 0}};

UID::UID (const std::string& value) {
    const char* str = value.c_str();
    *this = UID(str);
}

// TODO: optimize this
UID::UID (const char* value) {
    std::string name = value;
    std::unordered_map<std::string, uint64_t>::iterator ff = string_list.find(name);
    if(ff == string_list.end()){
        uint64_t key = string_pool.size();
        char* newstr = string_pool.AddNew(name.size() + 1);

        string_list.emplace(name, key);
        strcpy(newstr, name.c_str());
        
        this->key = key;
    } else {
        this->key = ff->second;
    }
}

UID::operator std::string() const {
    return string_pool.begin() + key;
}

UID::operator char const*() const {
    return string_pool.begin() + key;
}

}