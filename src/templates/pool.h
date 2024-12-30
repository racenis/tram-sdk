// Tramway Drifting and Dungeon Exploration Simulator SDK Runtime

#ifndef TRAM_SDK_TEMPLATES_POOL_H
#define TRAM_SDK_TEMPLATES_POOL_H

#include <string>
#include <iostream>
#include <cassert>

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
protected:
    std::string poolName;
    uint64_t poolSize;
    uint64_t maxSize;
    T* first;
    T* last;
    T* lastfree;
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

    Pool(std::string name, uint64_t initialSize, bool skipFirst = false){
        poolName = name;
        maxSize = initialSize;
        poolSize = skipFirst ? 1 : 0;

        first = static_cast<T*>(::operator new((initialSize * sizeof(T)) + sizeof(uint64_t) * 2));
        last = first;
        lastfree = first;

        if (skipFirst) {
            last++;
            lastfree++;
        }
        
        *((uint64_t*)last) = 0;
        *(((uint64_t*)last) + 1) = 0;
    };
    
    template <typename... Args>
    T* AddNew(Args&&... args){
        if(poolSize == maxSize){
            std::cout << "Pool " << poolName << " out of space!" << std::endl;
            abort();
        }

        T* newobj;

        if(lastfree != last){
            newobj = lastfree;
            uint64_t* skip = reinterpret_cast<uint64_t*>(lastfree);
            skip++;
            T** skip2 = reinterpret_cast<T**>(skip);
            lastfree = *skip2;
        } else {
            newobj = lastfree;
            last++;
            lastfree++;
            
            *((uint64_t*)last) = 0;
            *(((uint64_t*)last) + 1) = 0;
        }
        
        new(newobj) T(std::forward<Args>(args)...);
        
        poolSize++;

        return newobj;
    };
    
    void Remove(T* removeptr){
        assert(removeptr >= first && removeptr < last); // pointer is in pool
        removeptr->~T(); // destruct
        uint64_t* skip = reinterpret_cast<uint64_t*>(removeptr);
        *skip = 0; // mark as empty
        skip++;
        T** nextfree = reinterpret_cast<T**>(skip);
        *nextfree = lastfree; // add pointer to previous free place
        lastfree = removeptr;
        poolSize--;
    };
    T& operator[](size_t index) { return *(first + index); } // note that there is no checking for whether the index is valid
    T* GetFirst() {return first;};   // yeet?
    T* GetLast() {return last;};     // also yeet?
    iterator begin() {auto ptr = first; while (*((uint64_t*)ptr) == 0 && ptr < last) ptr++; return ptr;};
    iterator end() {return last;};
    size_t GetSize() const {return poolSize;}; // yeet too?
    size_t size() const {return poolSize;};
    size_t index(const T* ptr) const {return ptr - first;};
    
    /// Checks if an object can be accessed through iteration.
    bool validate (const T* ptr) const {
        return ptr >= first && ptr <= last && *((uint64_t*)ptr) != 0;
    }

    // make sure that there will be enough room for the empty place marker and free list pointer
    static_assert(sizeof(T) >= sizeof(T*) + sizeof(uint64_t));
    //static_assert(sizeof(T) % sizeof(uint64_t) == 0); // idk what this is
};

template <typename T>
class PoolProxy {
public:
    template <typename... Args>
    static T* New(Args&&... args){return pool.AddNew(std::forward<Args>(args)...);}
    static void Delete(T* obj){pool.Remove(obj);}
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