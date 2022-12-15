// TRAMWAY DRIFT AND DUNGEON EXPLORATION SIMULATOR 2022
// All rights reserved.

#ifndef EVENT_H
#define EVENT_H

#include <cstdint>

// TODO: remove this
#include <templates.h>

namespace Core {
    typedef uint16_t event_t;
    typedef uint64_t id_t;
    
    // TODO: remove this
    class EntityComponent;
    
    // CHANGES IN EVENTS:
    // - AddListener will instead take the type of the event,
    //   as it does now, as well as a class pointer and a
    //   method pointer. this will be the new way to call
    //   the message handler.
    // - If the class pointer is not provided, then it's
    //   assumed that a function will be called.
    // - Remove distance and location from events
    // - Hide the Listener struct implementations
    // - Convert event "listener" pointer to opaque
    // - Add a thingy to register new event types

    struct Event {
        enum Type : event_t {
            KEYPRESS,
            KEYDOWN,
            KEYUP,
            CURSORPOS,
            LAST_EVENT
        };

        struct Listener {
            EntityComponent* ent = nullptr; // TODO: rename this
            uint64_t poster = 0;
            //glm::vec3 location;
            float dist = 0.0f;
            event_t type = 0;
        };

        static void Post (Event &event);
        static void Dispatch();
        static Event::Listener* AddListener(Event::Type type);
        static void RemoveListener(Event::Listener* listener);

        event_t type;
        event_t subtype;
        //glm::vec3 location;
        uint64_t posterID;
        void* data;

        // TODO: move into implementation .cpp
        static Pool<Event::Listener> listeners;
        static std::vector<std::vector<Event::Listener*>> dispatch_table;
        static Queue<Event> queue;
    };
}

#endif // EVENT_H