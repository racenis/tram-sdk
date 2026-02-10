// Tramway Drifting and Dungeon Exploration Simulator SDK Runtime

#ifndef TRAM_SDK_TEMPLATES_POOL_H
#define TRAM_SDK_TEMPLATES_POOL_H

#include <string>
#include <iostream>
#include <cassert>
#include <cstdint>

/* this is one of the oldest files in the library, hence why it looks so awful.
 * it probably would be a good idea to clean up the code, but editing this file
 * causes the whole library to re-compile and I don't really feel like doing it
 * now. also adding this comment caused the whole library to recompile. also
 * adding this bit at the end caused the library to re-compile. I should
 * probably stop editing this file.
 */

namespace tram {

template <typename T>
class Pool {
public:
    struct iterator {
        iterator(T* ptr) : ptr(ptr) {}
        
        T& operator*() const { return *ptr; }
        T* operator->() { return ptr; }

        iterator& operator++() { do ptr++; while (*((uint64_t*)ptr) == 0 && *(((uint64_t*)ptr) + 1) != 0); return *this; }  

        iterator operator++(int) { iterator tmp = *this; ++(*this); return tmp; }

        friend bool operator== (const iterator& a, const iterator& b) { return a.ptr == b.ptr; };
        friend bool operator!= (const iterator& a, const iterator& b) { return a.ptr != b.ptr; };  
        
        T* ptr;
    };

    constexpr Pool(const char* name, uint64_t initial_size) {
        print_name = name;
        full_size = initial_size;
        current_size = 0;

        first = static_cast<T*>(::operator new((initial_size * sizeof(T)) + sizeof(uint64_t) * 2));
        last = first;
        last_free = first;
        
        *((uint64_t*)last) = 0;
        *(((uint64_t*)last) + 1) = 0;
    };
    
    constexpr Pool(const char* name, uint64_t initial_size, std::initializer_list<T> list) : Pool(name, initial_size) {
        for (const auto& entry : list) {
            make(entry);
        }
    }
    
    template <typename... Args>
    T* make(Args&&... args) {
        if (current_size == full_size) {
            std::cout << "Pool " << print_name << " out of space!" << std::endl;
            abort();
        }

        T* new_obj;

        if (last_free != last) {
            new_obj = last_free;
            uint64_t* skip = reinterpret_cast<uint64_t*>(last_free);
            skip++;
            T** skip2 = reinterpret_cast<T**>(skip);
            last_free = *skip2;
        } else {
            new_obj = last_free;
            last++;
            last_free++;
            
            *((uint64_t*)last) = 0;
            *(((uint64_t*)last) + 1) = 0;
        }
        
        new(new_obj) T(std::forward<Args>(args)...);
        
        current_size++;

        return new_obj;
    }
    
    void yeet(T* obj) {
        assert(obj >= first && obj < last); // pointer is in pool
        obj->~T(); // destruct
        uint64_t* skip = reinterpret_cast<uint64_t*>(obj);
        *skip = 0; // mark as empty
        skip++;
        T** next_free = reinterpret_cast<T**>(skip);
        *next_free = last_free; // add pointer to previous free place
        last_free = obj;
        current_size--;
    }
    
    T& operator[](size_t index) { return *(first + index); } // note that there is no checking for whether the index is valid
    
    iterator begin()  {auto ptr = first; while (*((uint64_t*)ptr) == 0 && ptr < last) ptr++; return ptr; }
    iterator end() { return last; }
    
    size_t size() const {return current_size;}
    size_t index(const T* ptr) const {return ptr - first;}
    
    /// Checks if an object can be accessed through iteration.
    bool validate (const T* ptr) const {
        return ptr >= first && ptr <= last && *((uint64_t*)ptr) != 0;
    }

    // aliases to not break old code. do not use for new code
    template <typename... Args>
    T* AddNew(Args&&... args) { return make(std::forward<Args>(args)...); }
    void Remove(T* ptr) { yeet(ptr); }
    size_t GetSize() const { return current_size; }
    T* GetFirst() { return first; }
    T* GetLast() { return last; }
    
protected:
    std::string print_name;
    uint64_t current_size;
    uint64_t full_size;
    T* first;
    T* last;
    T* last_free;

    // make sure that there will be enough room for the empty place marker and free list pointer
    static_assert(sizeof(T) >= sizeof(T*) + sizeof(uint64_t));
};

template <typename T>
class PoolProxy {
public:
    template <typename... Args>
    static T* make(Args&&... args){ return pool.AddNew(std::forward<Args>(args)...); }
    static void yeet(T* obj){ pool.Remove(obj); }
    
    // compatibility aliases
    template <typename... Args>
    static T* New(Args&&... args){ return make(std::forward<Args>(args)...); }
    static void Delete(T* obj){ yeet(obj); }
    
    static Pool<T>& GetPool(){return pool;}
protected:
    static Pool<T> pool;
};

template <typename T> using PoopProxy = PoolProxy<T>;

template <typename T>
class PoolPtr {
public:
    PoolPtr () { ptr = PoolProxy<T>::New(); }
    ~PoolPtr () { PoolProxy<T>::Delete(ptr); }
    T* GetResource() { return ptr; }
    T* operator->() { return ptr; }
    T& operator*() { return ptr; }
    explicit operator bool() { return ptr != nullptr; }
protected:
    T* ptr;
};

}

#endif  // TRAM_SDK_TEMPLATES_POOL_H