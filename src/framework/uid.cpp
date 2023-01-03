#include <framework/uid.h>

#include <cstring>
#include <framework/templates.h>

// TODO: yeet this
#include <unordered_map>

namespace Core {
    StackPool<char> stringPool("stringpool", 10000);
    std::unordered_map<std::string, uint64_t> stringHashMap;
    
    UID::UID(const std::string& value) {
        const char* str = value.c_str();
        *this = UID(str);
    }
    
    // TODO: optimize this
    UID::UID(const char*& value) {
        std::string name = value;
        std::unordered_map<std::string, uint64_t>::iterator ff = stringHashMap.find(name);
        if(ff == stringHashMap.end()){
            uint64_t key = stringPool.GetSize();
            char* newstr = stringPool.AddNew(name.size() + 1);

            stringHashMap.emplace(name, key);
            strcpy(newstr, name.c_str());
            //return key;
            this->key = key;
        } else {
            //return ff->second;
            this->key = ff->second;
        }
    }

    UID::UID(const uint64_t& value) {
        this->key = value;
    }
    
    UID::operator std::string() const {
        return stringPool.begin() + key;
    }
    
    UID::operator char const*() const {
        return stringPool.begin() + key;
    }
    
}