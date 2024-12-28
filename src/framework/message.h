// Tramway Drifting and Dungeon Exploration Simulator SDK Runtime

#ifndef TRAM_SDK_FRAMEWORK_MESSAGE_H
#define TRAM_SDK_FRAMEWORK_MESSAGE_H

#include <framework/uid.h>

namespace tram {

typedef uint32_t message_t;
typedef uint32_t id_t;

struct Value;
typedef Value value_t;

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
        SET_FLAG_ON,
        SET_FLAG_OFF,
        LAST_MESSAGE
    };

    static message_t Register(const char* name);
    static message_t GetType(name_t name);
    static name_t GetName(message_t type);
    static message_t GetLast();
    
    static void Send(const Message& message);
    static void Send(const Message& message, float delay);
    static void Dispatch();
    static void* AllocateData(size_t ammount);
    template <typename T> static T* AllocateData() { return (T*)AllocateData(sizeof(T)); }
    template <typename T> static T* AllocateData(const T& data) { T* ptr = (T*)AllocateData(sizeof(T)); *ptr = data; return ptr; }
    static void SetInterceptCallback(void(const Message&));

    message_t type;
    id_t sender;
    id_t receiver;
    
    union {
        void* data = nullptr;
        int32_t data_int;
        value_t* data_value;
    };
};

}

#endif // TRAM_SDK_FRAMEWORK_MESSAGE_H