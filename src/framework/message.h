// TRAMWAY DRIFT AND DUNGEON EXPLORATION SIMULATOR 2022
// All rights reserved.

#ifndef FRAMEWORK_MESSAGE_H
#define FRAMEWORK_MESSAGE_H

#include <cstdint>

#include <templates/queue.h>

namespace Core {
    typedef uint64_t message_t;
    typedef uint64_t id_t;

    struct Message {
        enum Type : uint64_t {
            PING,
            HELLO,
            ACTIVATE,
            GET_IN,
            GET_OUT,
            LAST_MESSAGE
        };

        // make this const ref
        static void Send (Message &message);
        static void Dispatch();

        message_t type;
        id_t receiverID;
        id_t senderID;
        void* data = nullptr;

        // TODO: make this not visible
        static Queue<Message> queue;
    };
}

#endif // FRAMEWORK_MESSAGE_H