// Tramway Drifting and Dungeon Exploration Simulator SDK Runtime

#ifndef TRAM_SDK_TEMPLATES_SPINLOCK_H
#define TRAM_SDK_TEMPLATES_SPINLOCK_H

#include <atomic>

/* I have no idea how useful this is.
 */

namespace tram {

class Spinlock {
public:
    void Lock() {
        while (lock.exchange(true));
    }
    
    void Unlock() {
        lock.store(false);
    }
private:
    std::atomic<bool> lock = {false};
};
    
}

#endif  // TRAM_SDK_TEMPLATES_SPINLOCK_H