// TRAMWAY DRIFT AND DUNGEON EXPLORATION SIMULATOR 2022
// All rights reserved.

#ifndef TEMPLATES_HASHMAP_H
#define TEMPLATES_HASHMAP_H

#include <framework/uid.h>
#include <iostream> // error message

namespace tram {
    template <typename T>
    class Hashmap {
    protected:
        typedef std::pair<uint64_t, T> PAIR;
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
            
            for (PAIR* it = first; it != last; it++) new (it) PAIR(); // wait what the fuck
        }
        
        T Find (UID key) {
            return Find(key.key);
        }
        
        T Find (uint64_t key) {
            uint64_t hash = key % max_size;
            
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
            Insert(key.key, value);
        }
        
        void Insert (uint64_t key, T value) {
            if(size == max_size){
                std::cout << "Hashmap " << name << " density reached!" << std::endl;
            }
            
            uint64_t hash = key % max_size;
            
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