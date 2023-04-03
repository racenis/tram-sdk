// TRAMWAY DRIFT AND DUNGEON EXPLORATION SIMULATOR 2022
// All rights reserved.

#ifndef TRAM_SDK_TEMPLATES_QUEUE_H
#define TRAM_SDK_TEMPLATES_QUEUE_H

#include <iostream>
#include <atomic>

namespace tram {

template <typename T>
class Queue {
public:
    Queue () = delete;
    Queue (const Queue&) = delete;
    Queue (Queue&&) = delete;
    Queue (const char* name, size_t count) : name(name), count(count) {
        T* allocated_memory = (T*)::operator new (count * sizeof(T));
        
        first = allocated_memory;
        last = allocated_memory;
        
        memory_start = allocated_memory;
        memory_end = allocated_memory + count;
        
        csize = 0;
    }
    
    /// Thread safe.
    template <typename... Args>
    void push (Args&&... args) {
        lock();
        
        if (count == csize) {
            std::cout << "Queue " << name << " out of space!" << std::endl;
            unlock();
            return;
        }
        
        new (last) T (std::forward<Args>(args)...);
        
        last++;
        csize++;
        
        // wrap around
        if (last == memory_end) {
            last = memory_start;
        }
        
        unlock();
    }
    
    void pop () {
        first->~T();
        
        first++;
        csize--;

        // wrap around
        if (first == memory_end) {
            first = memory_start;
        }
    }
    
    T& front () {
        return *first;
    }
    
    T& back () {
        if (T* elem = last - 1; elem < memory_start) {
            return *(memory_end - 1);
        } else {
            return *last;
        }
    }
    
    /// Thread safe.
    /// Copies front of the queue into value.
    /// @return True if there was an element into the front of the queue and it
    ///         was copied into value. Otherwise false.
    bool try_pop (T& value) {
        lock();
        
        if (csize == 0) {
            unlock();
            return false;
        }
        
        value = *first;
        pop();
        
        unlock();
        return true;
    }
    
    size_t size () { return csize; }

    void lock () { while (spinlock.exchange(true)); }
    void unlock () { spinlock.store(false); }
    
protected:
    const char* name;   // name of queue for log messages etc.
    size_t csize;       // how many elements are in queue right now
    size_t count;       // how many elements can be added to queue
    T* first;           // first element in queue
    T* last;            // one past last element in queue
    T* memory_start;    // beginning of allocated memory
    T* memory_end;      // end of allocated memory
    
    std::atomic<bool> spinlock = {false};
};

}

#endif  // TRAM_SDK_TEMPLATES_QUEUE_H