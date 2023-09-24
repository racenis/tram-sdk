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
        SIGNAL_LAST
    };
    
    Type type;
    
    name_t receiver;
    uint64_t data_int;
    
    union {
        message_t message_type;
        event_t event_type;
    };
    
    signal_t Register();
};
    
}

#endif // TRAM_SDK_FRAMEWORK_SIGNAL_H