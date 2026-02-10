// Tramway Drifting and Dungeon Exploration Simulator SDK Runtime

#ifndef TRAM_SDK_TEMPLATES_SPINLOCK_H
#define TRAM_SDK_TEMPLATES_SPINLOCK_H

#include <atomic>

/* I have no idea how useful this is.
 */

namespace tram {

class Spinlock {
public:
    void lock() {
        while (lock_atomic.exchange(true));
    }
    
    void unlock() {
        lock_atomic.store(false);
    }
    
    void Lock() { lock(); }
    void Unlock() { unlock(); }
private:
    std::atomic<bool> lock_atomic = {false};
};
    
}

#endif  // TRAM_SDK_TEMPLATES_SPINLOCK_H