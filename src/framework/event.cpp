// TRAMWAY DRIFT AND DUNGEON EXPLORATION SIMULATOR 2022
// All rights reserved.

#include <vector>

#include <templates/queue.h>
#include <templates/stackpool.h>
#include <templates/hashmap.h>

#include <framework/event.h>
#include <framework/entity.h>
#include <framework/entitycomponent.h>

namespace tram {

const size_t MAX_EVENT_TYPES = 100;
    
struct ListenerInfo {
    enum ListenerType : int64_t {
        LISTENER_COMPONENT,
        LISTENER_ENTITY,
        LISTENER_DATA_FUNCTION,
        LISTENER_FUNCTION
    };
    
    union {
        EntityComponent* component = nullptr;
        Entity* entity;
        void (*data_function)(Event& event, void* data);
        void (*function)(Event& event);
    };

    void* data;

    listener_t handle = -1;
    ListenerType type = LISTENER_COMPONENT;
};

static Queue<Event> event_queue ("event queue", 500);
static StackPool<char> data_pool ("event data pool", 2000);
//static std::vector<Pool<ListenerInfo>> listener_table(Event::LAST_EVENT, {"EVENTListnerPoo", 50});
static std::vector<std::vector<ListenerInfo>> listener_table(Event::LAST_EVENT, std::vector<ListenerInfo>());

static const char* event_names[MAX_EVENT_TYPES] = {
    "keypress",
    "keydown",
    "keyup",
    "keychar",
    "cursorpos",
    "tick",
    "selected",
    "look-at"
};

static event_t last_type = Event::LAST_EVENT;
static Hashmap<event_t> name_t_to_event_t("name_t_to_event_t", (MAX_EVENT_TYPES*2)+11);

/// Registers a new event type.
event_t Event::Register(const char* name) {
    // TODO: add a check that event with that name is not registered already!!!
    // TODO: also check if name is valid
    
    listener_table.push_back(std::vector<ListenerInfo>());
    event_names[last_type] = name;
    return last_type++;
}

event_t Event::GetType(name_t name) {
    event_t type = name_t_to_event_t.Find(name);
    
    if (!type) {
        for (event_t i = 0; i < last_type; i++) {
            if (event_names[i] == name) {
                name_t_to_event_t.Insert(name, i);
                return i;
            }
        }
    }
    
    return type;
}

name_t Event::GetName(event_t type) {
    return event_names[type];
}

event_t Event::GetLast() {
    return last_type;
}


static listener_t NewListenerHandle(event_t type) {
    static listener_t last_id = 0;
    
    listener_t id_part = (last_id++) << 32;
    listener_t event_part = type;
    
    return id_part | event_part;
}

/// Registers a listener.
listener_t Event::AddListener(event_t type, EntityComponent* component) {
    ListenerInfo new_listener;
    
    new_listener.component = component;
    new_listener.type = ListenerInfo::LISTENER_COMPONENT;
    new_listener.handle = NewListenerHandle(type);
    
    listener_table[type].push_back(new_listener);
    
    return new_listener.handle;
}

/// Registers a listener.
listener_t Event::AddListener(event_t type, Entity* entity) {
    ListenerInfo new_listener;
    
    new_listener.entity = entity;
    new_listener.type = ListenerInfo::LISTENER_ENTITY;
    new_listener.handle = NewListenerHandle(type);
    
    listener_table[type].push_back(new_listener);
    
    return new_listener.handle;
}

/// Registers a listener
listener_t Event::AddListener(event_t type, void* data, void (*data_function)(Event& event, void* data)) {
    ListenerInfo new_listener;
    
    new_listener.data = data;
    new_listener.data_function = data_function;
    new_listener.type = ListenerInfo::LISTENER_DATA_FUNCTION;
    new_listener.handle = NewListenerHandle(type);
    
    listener_table[type].push_back(new_listener);
    
    return new_listener.handle;
}

/// Registers a listener.
listener_t Event::AddListener(event_t type, void (*function)(Event& event)) {
    ListenerInfo new_listener;
    
    new_listener.function = function;
    new_listener.type = ListenerInfo::LISTENER_FUNCTION;
    new_listener.handle = NewListenerHandle(type);
    
    listener_table[type].push_back(new_listener);
    
    return new_listener.handle;
}

/// Deregisters a listener.
void Event::RemoveListener(listener_t listener_id) {
    // here I just do a simple linear search in the listener table
    // if it is too slow, then it could possibly be replaced with a binary search,
    // since each new handle is numerically larger than the previous, as well as
    // each listener being added to the end of the table makes it sorted.
    
    //listener_t id_part = listener_id >> 32;
    listener_t event_part = (listener_id << 32) >> 32;
    
    auto it = listener_table[event_part].begin();
    auto end = listener_table[event_part].end();
    
    while (it != end) {
        if (it->handle == listener_id) break;
        it++;
    }
    
    if (it == end) {
        std::cout << "Listener with key " << listener_id << "not found and not deleted!" << std::endl;
        return;
    }
    
    listener_table[event_part].erase(it);
}

/// Dispatches events from the event queue.
void Event::Dispatch() {
    while (event_queue.size()) {
        Event& event = event_queue.front();

        for (auto& listener : listener_table[event.type]) {
            switch (listener.type) {
                case ListenerInfo::LISTENER_COMPONENT:
                    listener.component->EventHandler(event);
                    break;
                case ListenerInfo::LISTENER_ENTITY:
                    listener.entity->EventHandler(event);
                    break;
                case ListenerInfo::LISTENER_DATA_FUNCTION:
                    listener.data_function(event, listener.data);
                    break;
                case ListenerInfo::LISTENER_FUNCTION:
                    listener.function(event);
                    break;
            }
        }

        event_queue.pop();
    }
    
    data_pool.Reset();
}

/// Adds an event to the event queue.
void Event::Post (const Event &event) {
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