// TRAMWAY DRIFT AND DUNGEON EXPLORATION SIMULATOR 2022
// All rights reserved.

#ifndef TRAM_SDK_TEMPLATES_SPINLOCK_H
#define TRAM_SDK_TEMPLATES_SPINLOCK_H

#include <atomic>

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