// Tramway Drifting and Dungeon Exploration Simulator SDK Runtime

#include <vector>

#include <templates/queue.h>
#include <templates/stackpool.h>
#include <templates/hashmap.h>

#include <framework/event.h>
#include <framework/entity.h>
#include <framework/entitycomponent.h>
#include <framework/logging.h>

/**
 * @struct tram::Event framework/event.h
 * 
 * Event data.
 * 
 * Fill in the struct fields and then use Event::Post() to send it out to
 * listeners.
 * 
 * @property tram::Event::type
 * Type of the event.
 * 
 * @property tram::Event::subtype
 * Arbitrary event subtype.
 * Some event types have subtypes with specific semantics, but if you create
 * your own event types, you can use the subtype field for anything.
 * 
 * @property tram::Event::poster_id
 * ID of the Entity that emitted the event.
 * Either set to the ID of an Entity, or can be set to zero.
 * 
 * @property tram::Event::data
 * Pointer to arbitrary data.
 * Can be set to a nullptr. Some event types have specific data, e.g. structs or
 * other types that they need their data pointers to be pointing to.
 * For your own event types, you can use this pointer for whatever purpose you
 * want.
 * If uncertain, set this to a nullptr.
 * 
 * @property tram::Event::data_int
 * Arbitrary data integer.
 * 
 * @property tram::Event::data_value
 * Pointer to a value_t.
 * 
 * @see https://racenis.github.io/tram-sdk/documentation/framework/event.html
 */

/**
 * @typedef uint16_t event_t
 * Event type number.
 * Can be set to any of the values in Event::Type, or you can use
 * Event::Register() to allocate new event types.
 */
 
 /**
 * @typedef uint32_t listener_t
 * Event listener handle.
 * Opaque handle, used to yeet event listeners.
 */
 
 /**
 * @class tram::EventListener framework/event.h
 * 
 * Smart wrapper class for Event listeners.
 * 
 * Calling the make() method will initialize the event listener, just like
 * calling Event::AddListener() will do.
 * Calling the clear() method will delete the listener, just like calling
 * Event::RemoveListener() will do.
 * 
 * When this class is destroyed, the event listener will also be automatically
 * removed.
 * 
 * You can also call the make() and clear() methods as much as you want, no
 * resources will be leaked. Any existing event listener will be removed before
 * a new one gets added.
 */
 
namespace tram {

const size_t MAX_EVENT_TYPES = 100;
    
struct ListenerInfo {
    enum ListenerType : uint32_t {
        LISTENER_COMPONENT,
        LISTENER_ENTITY,
        LISTENER_DATA_FUNCTION,
        LISTENER_FUNCTION,
        LISTENER_DELETED
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
static std::vector<std::vector<ListenerInfo>> new_listeners(Event::LAST_EVENT, std::vector<ListenerInfo>());

static const char* event_names[MAX_EVENT_TYPES] = {
    "none",
    "keypress",
    "keydown",
    "keyup",
    "keychar",
    "cursorpos",
    "frame",
    "tick",
    "selected",
    "look-at"
};

static event_t last_type = Event::LAST_EVENT;
static Hashmap<event_t> name_t_to_event_t("name_t_to_event_t", (MAX_EVENT_TYPES*2)+11);

/// Registers a new event type.
event_t Event::Register(const char* name) {
    if (UID::is_empty(name)) {
        Log(Severity::CRITICAL_ERROR, System::CORE, "Event name '{}' is empty", name);
    }
    
    if (!UID::no_quote(name)) {
        Log(Severity::CRITICAL_ERROR, System::CORE, "Event name '{}' contains invalid characters", name);
    }
    
    for (event_t i = 0; i < last_type; i++) {
        if (strcmp(event_names[i], name) != 0) continue;
        
        Log(Severity::CRITICAL_ERROR, System::CORE, "Event name '{}' already in use", name);
    }
    
    if (last_type >= MAX_EVENT_TYPES) {
        Log(Severity::CRITICAL_ERROR, System::CORE, "Event count limit exceeded when registering '{}'", name);
    }

    listener_table.push_back(std::vector<ListenerInfo>());
    new_listeners.push_back(std::vector<ListenerInfo>());
    
    event_names[last_type] = name;
    
    return last_type++;
}

/// Returns the event_t associated with a given name.
event_t Event::GetType(name_t name) {
    event_t type = name_t_to_event_t.Find(name);
    
    if (!type && name) {
        for (event_t i = 0; i < last_type; i++) {
            if (event_names[i] == name) {
                name_t_to_event_t.Insert(name, i);
                return i;
            }
        }
    }
    
    return type;
}

/// Returns the name that was associated with a given event_t.
name_t Event::GetName(event_t type) {
    if (type >= MAX_EVENT_TYPES) {
        Log(Severity::CRITICAL_ERROR, System::CORE, "Attempting index {} invalid event", type);
    }
    
    return event_names[type];
}

/// Returns the last event_t plus one.
/// Useful for iterating over all registered events, i.e. in a 
/// `while (++event < Event::GetLast());` or a similar loop.
event_t Event::GetLast() {
    return last_type;
}


static listener_t NewListenerHandle(event_t type) {
    static listener_t last_id = 0;
    
    assert(type < last_type);
    
    listener_t id_part = (last_id++) << 16;
    listener_t event_part = type;
    
    return id_part | event_part;
}

/// Registers a listener.
listener_t Event::AddListener(event_t type, EntityComponent* component) {
    ListenerInfo new_listener;
    
    assert(type < last_type);
    assert(component);
    
    new_listener.component = component;
    new_listener.type = ListenerInfo::LISTENER_COMPONENT;
    new_listener.handle = NewListenerHandle(type);
    
    new_listeners[type].push_back(new_listener);
    
    return new_listener.handle;
}

/// Registers a listener.
listener_t Event::AddListener(event_t type, Entity* entity) {
    ListenerInfo new_listener;
    
    assert(type < last_type);
    assert(entity);
    
    new_listener.entity = entity;
    new_listener.type = ListenerInfo::LISTENER_ENTITY;
    new_listener.handle = NewListenerHandle(type);
    
    new_listeners[type].push_back(new_listener);
    
    return new_listener.handle;
}

/// Registers a listener
listener_t Event::AddListener(event_t type, void* data, void (*data_function)(Event& event, void* data)) {
    ListenerInfo new_listener;
    
    assert(type < last_type);
    assert(data_function);
    
    new_listener.data = data;
    new_listener.data_function = data_function;
    new_listener.type = ListenerInfo::LISTENER_DATA_FUNCTION;
    new_listener.handle = NewListenerHandle(type);
    
    new_listeners[type].push_back(new_listener);
    
    return new_listener.handle;
}

/// Registers a listener.
listener_t Event::AddListener(event_t type, void (*function)(Event& event)) {
    ListenerInfo new_listener;
    
    assert(type < last_type);
    assert(function);
    
    new_listener.function = function;
    new_listener.type = ListenerInfo::LISTENER_FUNCTION;
    new_listener.handle = NewListenerHandle(type);
    
    new_listeners[type].push_back(new_listener);
    
    return new_listener.handle;
}

/// Deregisters a listener.
void Event::RemoveListener(listener_t listener_id) {
    // here I just do a simple linear search in the listener table
    // if it is too slow, then it could possibly be replaced with a binary search,
    // since each new handle is numerically larger than the previous, as well as
    // each listener being added to the end of the table makes it sorted.

    listener_t event_part = (listener_id << 16) >> 16;
    
    // why is this code so weird
    auto it = listener_table[event_part].begin();
    auto end = listener_table[event_part].end();
    
    // this bit searches for listener in the main table
    while (it != end) {
        if (it->handle == listener_id) break;
        it++;
    }
    
    // this checks if it wasn't found
    if (it == end) {
        it = new_listeners[event_part].begin();
        end = new_listeners[event_part].end();
        
        // this here will search it in the new listener table
        while (it != end) {
            if (it->handle == listener_id) break;
            it++;
        }
        
        if (it == end) {
            Log(Severity::WARNING, System::CORE, "Listener with key {} not found and not deleted", listener_id);
            return;
        }
        
        // deletes it if it is found
        new_listeners[event_part].erase(it);
        
        return;
    }
    
    // otherwise mark it as deleted for removal later
    it->type = ListenerInfo::LISTENER_DELETED;
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
                case ListenerInfo::LISTENER_DELETED:
                    break;
            }
        }

        event_queue.pop();
    }
    
    data_pool.Reset();
    
    // the reason why we mark listeners for deletion and peform the deletions
    // later is because we are using std::vectors for the listener tables.
    // if you are iterating through a vector and delete stuff from it, you might
    // run into some memory errors, which could happen if you delete an event
    // listener from an event handler.
    // same applies to inserting new event listeners.
    
    // remove listeners pending for deletion
    for (auto& table : listener_table) {
        std::erase_if(table, [](ListenerInfo& info) {
            return info.type == ListenerInfo::LISTENER_DELETED;
        });
    }
    
    // add in listeners pending for insertion
    // TODO: change this to using zip when C++23 comes out
    for (size_t event = 0; event < new_listeners.size(); event++) {
        for (auto& listener : new_listeners[event]) {
            listener_table[event].push_back(listener);
        }
        new_listeners[event].clear();
    }
}

/// Adds an event to the event queue.
void Event::Post (const Event &event) {
    event_queue.push(event);
}

/// Allocates space for event data.
/// This allocation is useful for storing the additional data (Event::data pointer),
/// for events, since all allocated space will be cleared once all events have been 
/// dispatched.
/// @note Only store POD data types in the allocated memory. Either that, or
///       prepare for memory leaks. Altough an IDE might complain that a value_t
///       is not POD, you can ignore the warning. Overwriting a value_t
///       shouldn't cause memory leaks.
void* Event::AllocateData(size_t ammount) {
    return data_pool.AddNew(ammount);
}

EventListener::~EventListener() {
    clear();    
}

/// Essentially same as Event::AddListener().
void EventListener::make(event_t event, Entity* parent) {
    clear();
    listener = Event::AddListener(event, parent);
}

/// Essentially same as Event::AddListener().
void EventListener::make(event_t event, EntityComponent* parent) {
    clear();
    listener = Event::AddListener(event, parent);
}

/// Essentially same as Event::AddListener().
void EventListener::make(event_t event, void (*handler)(Event& event)) {
    clear();
    listener = Event::AddListener(event, handler);
}

/// Essentially same as Event::RemoveListener().
void EventListener::clear() {
    if (listener) {
        Event::RemoveListener(listener);
    }

    listener = 0;
}

}