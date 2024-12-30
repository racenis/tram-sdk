// Tramway Drifting and Dungeon Exploration Simulator SDK Runtime

#ifndef TRAM_SDK_TEMPLATES_STACKPOOL_H
#define TRAM_SDK_TEMPLATES_STACKPOOL_H

#include <string>
#include <iostream>

namespace tram {

/// Allocator.
/// Very simple allocator for data types that don't need their destructors
/// called. Just call allocate() and specify the size of the array that you want
/// to allocated. Use the pointer to write and store data. After you are done,
/// you can reset all of the allocated memory with reset().
template <typename T>
class StackPool {
public:
    constexpr StackPool(std::string name, size_t size) {
        this->name = name;
        this->available_size = size;
        this->allocated_size = 0;

        first = static_cast<T*>(::operator new(size * sizeof(T)));
        last = first;
    }
    
    constexpr StackPool(std::string name, size_t max_size, std::initializer_list<T> list) : StackPool(name, max_size) {
        for (const auto& entry : list) {
            *AddNew(1) = entry; // not good, but works
        }
    }

    T* AddNew(size_t units) {
        if (allocated_size + units > available_size) {
            std::cout << "StackPool " << name << " out of space!" << std::endl;
            return nullptr;
        }

        T* allocation = last;
        
        allocated_size += units;
        last += units;

        return allocation;
    }
    
    T* allocate(size_t units) {
        return AddNew(units);
    }
    
    void Reset() {
        allocated_size = 0;
        last = first;
    }
    
    void reset() {
        Reset();
    }
    
    size_t size() { return allocated_size; }
    T* begin() { return first; }
    T* end() { return last; }

protected:
    std::string name;
    size_t allocated_size;
    size_t available_size;
    T* first;
    T* last;
};

}

#endif  // TRAM_SDK_TEMPLATES_STACKPOOL_H