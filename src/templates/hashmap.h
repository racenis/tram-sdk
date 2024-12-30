// Tramway Drifting and Dungeon Exploration Simulator SDK Runtime

#ifndef TRAM_SDK_TEMPLATES_HASHMAP_H
#define TRAM_SDK_TEMPLATES_HASHMAP_H

#include <framework/uid.h>
#include <iostream> // error message
#include <cstring>  // memset

/* instead of having two sets of each method, one with uint32_t and the other
 * with UID, maybe we could create some kind of a struct called Key and then we
 * could add a uint32_t converter to it and then we could add a constructor from
 * both uint32_t and UID and then we could just have only a single of each
 * method and all of the conversions would happen automatically
 * 
 * TODO: investigate
 */

namespace tram {

template <typename T>
class Hashmap {
public:
    constexpr Hashmap(std::string name, size_t max_size) {
        this->name = name;
        this->max_size = max_size;
        this->hash_parameter = max_size * 2;
        
        size_t memory_size = ((max_size * 2) + padding) * sizeof(Record);
        
        char* memory = (char*)::operator new(memory_size);
        
        first = (Record*) memory;
        last = (Record*) (memory + memory_size);
        
        memset(memory, 0, memory_size);
    }
    
    constexpr Hashmap(std::string name, size_t max_size, std::initializer_list<std::pair<uint32_t, T>> list) : Hashmap(name, max_size) {
        for (const auto& entry : list) {
            Insert(entry.first, entry.second);
        }
    }
    
    T Find(UID key) {
        return Find(key.key);
    }
    
    T Find(uint32_t key) {
        uint32_t hash = key % hash_parameter;
        
        Record* candidate = first + hash;
        
        while (candidate != last) {
            if (!(candidate->flags & (FLAG_DELETED | FLAG_RECORD))) {
                break;
            }
            
            if (candidate->key == key) {
                if (candidate->flags & FLAG_DELETED) {
                    break;
                } else {
                    return candidate->value;
                }
                
            }
            
            candidate++;
        }
        
        return T();
    }
    
    bool Exists(UID key) {
        return Exists(key.key);
    }
    
    bool Exists(uint32_t key) {
        uint32_t hash = key % hash_parameter;
        
        Record* candidate = first + hash;
        
        while (candidate != last) {
            if (!(candidate->flags & (FLAG_DELETED | FLAG_RECORD))) {
                break;
            }
            
            if (candidate->key == key) {
                if (candidate->flags & FLAG_DELETED) {
                    break;
                } else {
                    return true;
                }
                
            }
            
            candidate++;
        }
        
        return false;
    }
    
    void Insert(UID key, T value) {
        Insert(key.key, value);
    }
    
    void Insert(uint32_t key, T value) {
        if (size == max_size) {
            std::cout << "Hashmap " << name << " density reached!" << std::endl;
        }
        
        uint32_t hash = key % hash_parameter;
        
        Record* candidate = first + hash;
        
        while (candidate != last) {
            if (candidate->flags & FLAG_RECORD) {
                if (candidate->key == key) {
                    candidate->value.~T();
                    size--;
                    break;
                } else {
                    candidate++;
                    continue;
                }
            }
            
            if (candidate->flags & FLAG_DELETED) {
                break;
            }
            
            break;
        }
        
        if (candidate == last) {
            std::cout << "Hashmap " << name << " overflow!" << std::endl;
            abort();
        }
        
        size++;
        
        candidate->key = key;
        candidate->value = value;
        candidate->flags = FLAG_RECORD;
    }
    
    void Remove(UID key) {
        Remove(key.key);
    }
    
    void Remove(uint32_t key) {
        uint32_t hash = key % hash_parameter;
        
        Record* candidate = first + hash;
        
        while (candidate != last) {
            if (!(candidate->flags & (FLAG_DELETED | FLAG_RECORD))) {
                return;
            }
            
            if (candidate->key == key) {
                if (candidate->flags & FLAG_DELETED) {
                    return;
                } else {
                    candidate->value.~T();
                    candidate->flags = FLAG_DELETED;
                    size--;
                    return;
                }
                
            }
            
            candidate++;
        }
    }
    
    T& operator[](UID key) {
        return operator[](key.key);
    }
    
    T& operator[](uint32_t key) {
        uint32_t hash = key % hash_parameter;
        
        Record* candidate = first + hash;
        
        while (candidate != last) {
            if (candidate->key == key) {
                return candidate->value;
            }
            
            if (candidate->flags & FLAG_RECORD) {
                candidate++;
                continue;
            }
            
            if (candidate->flags & FLAG_DELETED) {
                break;
            }
            
            break;
        }
        
        if (candidate == last) {
            std::cout << "Hashmap " << name << " overflow!" << std::endl;
            abort();
        }
        
        size++;
        
        if (size == max_size) {
            std::cout << "Hashmap " << name << " density reached!" << std::endl;
        }
        
        candidate->key = key;
        candidate->value = T();
        candidate->flags = FLAG_RECORD;
        
        return candidate->value;
    }
    
protected:
    struct Record {
        uint32_t key = 0;
        uint32_t flags = 0;
        T value;
    };
    const size_t padding = 10;

    enum {
        FLAG_RECORD = 1,
        FLAG_DELETED = 2
    };

    std::string name;
    size_t size = 0;
    size_t max_size = 0;
    uint32_t hash_parameter = 0;
    Record* first = nullptr;
    Record* last = nullptr;
};

}

#endif // TRAM_SDK_TEMPLATES_HASHMAP_H