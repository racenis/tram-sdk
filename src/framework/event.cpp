// TRAMWAY DRIFT AND DUNGEON EXPLORATION SIMULATOR 2022
// All rights reserved.

#include <vector>

#include <templates/queue.h>
#include <templates/stackpool.h>

#include <framework/event.h>
#include <framework/entitycomponent.h>

namespace tram {

struct ListenerInfo {
    enum ListenerType : int64_t {
        LISTENER_COMPONENT,
        LISTENER_ENTITY,
        LISTENER_FUNCTION
    };
    
    union {
        EntityComponent* component = nullptr;
        Entity* entity;
        void (*function)(Event& event);
    };

    ListenerType type = LISTENER_COMPONENT;
};

static Queue<Event> event_queue ("event queue", 500);
static StackPool<char> data_pool ("event data pool", 2000);
static std::vector<Pool<ListenerInfo>> listener_table(Event::LAST_EVENT, {"EVENTListnerPoo", 50});

/// Registers a new event type.
event_t Event::Register() {
    auto new_event_id = listener_table.size();
    listener_table.emplace_back(std::string("Event Listener Pool ") + std::to_string(new_event_id), 50);
    return new_event_id;
}

static listener_t EncodeListenerHandle (event_t type, ListenerInfo* entry) {
    listener_t handle = type;
    handle <<= 48;
    handle |= (listener_t)entry;
    return handle;
}

/// Registers a listener.
listener_t Event::AddListener(event_t type, EntityComponent* component) {
    ListenerInfo* entry = listener_table[type].AddNew();
    entry->component = component;
    entry->type = ListenerInfo::LISTENER_COMPONENT;
    return EncodeListenerHandle(type, entry);
}

/// Registers a listener.
listener_t Event::AddListener(event_t type, Entity* entity) {
    ListenerInfo* entry = listener_table[type].AddNew();
    entry->entity = entity;
    entry->type = ListenerInfo::LISTENER_COMPONENT;
    return EncodeListenerHandle(type, entry);
}

/// Registers a listener.
listener_t Event::AddListener(event_t type, void (*function)(Event& event)) {
    ListenerInfo* entry = listener_table[type].AddNew();
    entry->function = function;
    entry->type = ListenerInfo::LISTENER_FUNCTION;
    return EncodeListenerHandle(type, entry);
}

/// Deregisters a listener.
void Event::RemoveListener(listener_t listener_id) {
    event_t type = listener_id >> 48;
    ListenerInfo* entry = (ListenerInfo*)((listener_id << 16) >> 16);
    listener_table[type].Remove(entry);
}

/// Dispatches events from the event queue.
void Event::Dispatch(){
    while (event_queue.size()) {
        Event& event = event_queue.front();
        
        for (auto& listener : listener_table[event.type]) {
            switch (listener.type) {
                case ListenerInfo::LISTENER_COMPONENT:
                    listener.component->EventHandler(event);
                    break;
                case ListenerInfo::LISTENER_ENTITY:
                    std::cout << "NO HANDLERS FOR ENTITIES YET!!" << std::endl;
                    abort();
                    break;
                case ListenerInfo::LISTENER_FUNCTION:
                    listener.function (event);
                    break;
            }
        }

        event_queue.pop();
    }
}

/// Adds an event to the event queue.
void Event::Post (const Event &event){
    event_queue.push(event);
}

/// Allocates space for event data.
/// This allocation is useful for storing the additional data (Event::data pointer),
/// for events, since all allocated space will be cleared once all events have been 
/// dispatched.
/// @note Only store POD data types in the allocated memory.
void* Event::AllocateData (size_t ammount) {
    return data_pool.AddNew(ammount);
}

EventListener::~EventListener() {
    clear();    
}

void EventListener::make(event_t event, Entity* parent) {
    clear();
    listener = Event::AddListener(event, parent);
}

void EventListener::make(event_t event, EntityComponent* parent) {
    clear();
    listener = Event::AddListener(event, parent);
}

void EventListener::clear() {
    if (listener) {
        Event::RemoveListener(listener);
    }

    listener = 0;
}

}