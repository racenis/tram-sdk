// TRAMWAY DRIFT AND DUNGEON EXPLORATION SIMULATOR 2022
// All rights reserved.

#ifndef FRAMEWORK_EVENT_H
#define FRAMEWORK_EVENT_H

#include <cstdint>

namespace Core {
    typedef uint16_t event_t;
    typedef uint64_t id_t;
    typedef uint64_t listener_t;
    
    class EntityComponent;
    class Entity;

    template <event_t event> struct EventDataType { void* type; };

    struct Event {
        enum Type : event_t {
            KEYPRESS,
            KEYDOWN,
            KEYUP,
            CURSORPOS,
            LAST_EVENT
        };

        static event_t Register();
        static void Post (const Event &event);
        static void Dispatch();
        static listener_t AddListener(event_t type, EntityComponent* component);
        static listener_t AddListener(event_t type, Entity* entity);
        static listener_t AddListener(event_t type, void (*handler)(Event& event));
        static void RemoveListener(listener_t listener_id);

        /// Experimental template function.
        /*
        template<event_t event_type>
        static decltype(EventDataType<event_type>().type)& RetrieveData(Event& event) {
            assert(event_type == event.type);
            return *((decltype(EventDataType<event_type>().type)*)event.data);
        }
        */

        event_t type;
        event_t subtype;
        id_t poster_id;
        void* data;
    };
    
    template<event_t EVENT, typename T>
    class EventListener {
    public:
        EventListener (T* parent) : parent(parent) {}
        ~EventListener () { clear(); }
        void make() { clear(); listener = Event::AddListener(EVENT, parent); }
        void clear() { if (listener) Event::RemoveListener(listener); listener = 0; }
        explicit operator bool() { return listener != 0; }
    protected:
        T* parent = nullptr;
        listener_t listener = 0;
    };
}

#endif // FRAMEWORK_EVENT_H