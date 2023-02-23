// TRAMWAY DRIFT AND DUNGEON EXPLORATION SIMULATOR 2022
// All rights reserved.

#ifndef TEMPLATES_HASHMAP_H
#define TEMPLATES_HASHMAP_H

#include <framework/core.h>
#include <iostream> // error message

namespace Core {
    template <typename T>
    class Hashmap {
    protected:
        typedef std::pair<UID, T> PAIR;
        const size_t padding = 10;

        std::string name;
        size_t size = 0;
        size_t max_size = 0;
        PAIR* first = nullptr;
        PAIR* last = nullptr;
    public:
        Hashmap (std::string name, size_t max_size) {
            this->name = name;
            this->max_size = max_size;
            
            size_t memory_size = ((max_size * 2) + padding) * sizeof(PAIR);
            
            char* memory = (char*)::operator new(memory_size);
            
            first = (PAIR*) memory;
            last = (PAIR*) (memory + memory_size);
            
            for (PAIR* it = first; it != last; it++) new (it) PAIR();
        }
        
        T Find (UID key) {
            uint64_t hash = key.key % max_size;
            
            PAIR* candidate = first + hash;
            
            while (candidate != last) {
                if (candidate->first == key) {
                    return candidate->second;
                }
                
                if (candidate->first == 0) {
                    break;
                }
                
                candidate++;
            }
            
            return T();
        }
        
        void Insert (UID key, T value) {
            if(size == max_size){
                std::cout << "Hashmap " << name << " density reached!" << std::endl;
            }
            
            uint64_t hash = key.key % max_size;
            
            PAIR* candidate = first + hash;
            
            while (candidate != last) {
                if (candidate->first == key) {
                    candidate->second.~T();
                    break;
                }
                
                if (candidate->first == 0) {
                    break;
                }
                
                candidate++;
            }
            
            if (candidate == last) {
                std::cout << "Hashmap " << name << " overflow!" << std::endl;
                abort();
            }
            
            candidate->first = key;
            candidate->second = value;
        }
    };

}

#endif // TEMPLATES_HASHMAP_H