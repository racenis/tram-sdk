// TRAMWAY DRIFT AND DUNGEON EXPLORATION SIMULATOR 2022
// All rights reserved.

#ifndef TRAM_SDK_FRAMEWORK_MESSAGE_H
#define TRAM_SDK_FRAMEWORK_MESSAGE_H

#include <framework/uid.h>

namespace tram {

typedef uint64_t message_t;
typedef uint32_t id_t;

struct Value;

struct Message {
    enum Type : message_t {
        NONE,
        PING,
        MOVE_PICK_UP,
        OPEN,
        CLOSE,
        LOCK,
        UNLOCK,
        TOGGLE,
        KILL,
        TRIGGER,
        START,
        STOP,
        ACTIVATE,
        ACTIVATE_ONCE,
        SELECT,
        SET_PROGRESS,
        SET_ANIMATION,
        LAST_MESSAGE
    };

    static message_t Register(const char* name);
    static message_t GetType(name_t name);
    static name_t GetName(message_t type);
    
    
    static void Send(const Message& message);
    static void Send(const Message& message, uint32_t when);
    static void Dispatch();
    static void* AllocateData(size_t ammount);

    message_t type;
    id_t sender;
    id_t receiver;
    
    union {
        void* data = nullptr;
        int64_t data_int;
        Value* data_value;
    };
};

}

#endif // TRAM_SDK_FRAMEWORK_MESSAGE_H