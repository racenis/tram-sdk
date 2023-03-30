// TRAMWAY DRIFT AND DUNGEON EXPLORATION SIMULATOR 2022
// All rights reserved.

#ifndef FRAMEWORK_MESSAGE_H
#define FRAMEWORK_MESSAGE_H

#include <cstdint>

namespace tram {
    typedef uint64_t message_t;
    typedef uint64_t id_t;

    struct Message {
        enum Type : message_t {
            PING,
            HELLO,
            ACTIVATE,
            GET_IN,
            GET_OUT,
            KILL,
            LAST_MESSAGE
        };

        static message_t Register ();
        static void Send (const Message &message);
        static void Send (const Message &message, uint32_t when);
        static void Dispatch ();
        static void* AllocateData (size_t ammount);

        message_t type;
        id_t receiver;
        id_t sender;
        void* data = nullptr;
    };
}

#endif // FRAMEWORK_MESSAGE_H