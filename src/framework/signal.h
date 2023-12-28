// TRAMWAY DRIFT AND DUNGEON EXPLORATION SIMULATOR 2022
// All rights reserved.

#ifndef TRAM_SDK_FRAMEWORK_SIGNAL_H
#define TRAM_SDK_FRAMEWORK_SIGNAL_H

#include <framework/message.h>
#include <framework/event.h>
#include <framework/uid.h>

namespace tram {

typedef uint32_t signal_t;
    
struct Signal {
    enum Type : signal_t {
        NONE,
        SPAWN,
        KILL,
        ACTIVATE,
        USE,
        LOCKED_USE,
        OPEN,
        CLOSE,
        END_OPEN,
        END_CLOSE,
        TRIGGER,
        ENTER_TRIGGER,
        EXIT_TRIGGER,
        LAST_SIGNAL
    };
    
    signal_t type = NONE;
    
    name_t receiver;
    int64_t data_int;
    
    float delay;
    int limit;
    
    message_t message_type;
    
    static signal_t Register(const char* name);
    static signal_t GetType(name_t name);
    static name_t GetName(signal_t type);
};

class SignalTable {
public:
    void Fire(signal_t signal, id_t sender);
    void Add(const Signal& signal);
public:
    Signal signals[10];
    size_t signal_count = 0;
};
    
}

#endif // TRAM_SDK_FRAMEWORK_SIGNAL_H