// Tramway Drifting and Dungeon Exploration Simulator SDK Runtime

#ifndef TRAM_SDK_TEMPLATES_STACKPOOL_H
#define TRAM_SDK_TEMPLATES_STACKPOOL_H

#include <string>
#include <iostream>

namespace tram {

template <typename T>
class StackPool {
public:
    StackPool(std::string name, size_t initialSize) {
        poolName = name;
        maxSize = initialSize;
        poolSize = 0;

        first = static_cast<T*>(::operator new(initialSize * sizeof(T)));
        last = first;
    }
    
    StackPool(std::string name, size_t max_size, std::initializer_list<T> list) : StackPool(name, max_size) {
        for (const auto& entry : list) {
            *AddNew(1) = entry; // not good, but works
        }
    }

    T* AddNew(size_t units) {
        if (poolSize + units > maxSize) {
            std::cout << "StackPool " << poolName << " out of space!" << std::endl;
            return nullptr;
        }

        T* newobj;

        newobj = last;
        poolSize += units;
        last += units;

        return newobj;
    }
    
    void Reset() {
        //return;
        poolSize = 0;
        last = first;
    }
    
    size_t size() { return poolSize; }
    T* begin() { return first; }
    T* end() { return last; }

protected:
    std::string poolName;
    size_t poolSize;
    size_t maxSize;
    T* first;
    T* last;
};

}

#endif  // TRAM_SDK_TEMPLATES_STACKPOOL_H