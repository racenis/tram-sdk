// Tramway Drifting and Dungeon Exploration Simulator SDK Runtime

#ifndef TRAM_SDK_FRAMEWORK_EVENT_H
#define TRAM_SDK_FRAMEWORK_EVENT_H

#include <cstdint>
#include <cstddef>
#include <type_traits>

#include <framework/uid.h>

namespace tram {

typedef uint16_t event_t;
typedef uint32_t id_t;
typedef uint32_t listener_t;

class EntityComponent;
class Entity;

struct Value;
typedef Value value_t;

struct Event {
    enum Type : event_t {
        NONE,           //< Invalid event type
        KEYPRESS,       //< Keyboard key still pressed
        KEYDOWN,        //< Keyboard key pressed
        KEYUP,          //< Keyboard key released
        KEYCHAR,        //< Keyboard input registered
        CURSORPOS,      //< Cursor moved
        FRAME,          //< Emitted every frame
        TICK,           //< Emitted every tick
        SELECTED,       //< Cursor on top of an interactable object
        LOOK_AT,        //< Entity is looking in a direction
        LAST_EVENT
    };

    static event_t Register(const char* name);
    static event_t GetType(name_t name);
    static name_t GetName(event_t type);
    static event_t GetLast();
    
    static void Post(const Event &event);
    static void Dispatch();
    
    static listener_t AddListener(event_t type, EntityComponent* component);
    static listener_t AddListener(event_t type, Entity* entity);
    static listener_t AddListener(event_t type, void* data, void (*handler)(Event& event, void* data));
    static listener_t AddListener(event_t type, void (*handler)(Event& event));
    
    static void RemoveListener(listener_t listener_id);
    
    static void* AllocateData(size_t ammount);
    template <typename T> static T* AllocateData() { return (T*)AllocateData(sizeof(T)); }
    template <typename T> static T* AllocateData(const T& data) { T* ptr = (T*)AllocateData(sizeof(T)); *ptr = data; return ptr; }

    event_t type;
    event_t subtype;
    id_t poster;
    
    union {
        void* data = nullptr;
        int32_t data_int;
        value_t* data_value;
    };
};

class EventListener {
public:
    ~EventListener();
    void make(event_t event, Entity* parent);
    void make(event_t event, EntityComponent* parent);
    void make(event_t event, void (*handler)(Event& event));
    void clear();
    explicit operator bool() { return listener != 0; }
protected:
    listener_t listener = 0;
};

}

#endif // TRAM_SDK_FRAMEWORK_EVENT_H