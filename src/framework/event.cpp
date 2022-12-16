#include <framework/event.h>
#include <framework/entitycomponent.h>

namespace Core {
    struct ListenerInfo {
        enum ListenerType {
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
    
    Queue<Event> EVENT_QUEUE("event queue", 500);
    std::vector<Pool<ListenerInfo>> LISTENER_TABLE;

    event_t Event::Register() {
        auto new_event_id = LISTENER_TABLE.size();
        LISTENER_TABLE.emplace_back(std::string("Event Listener Pool ") + std::to_string(new_event_id), 50);
        return new_event_id;
    }
    
    listener_t EncodeListenerHandle (event_t type, ListenerInfo* entry) {
        listener_t handle = type;
        handle <<= 48;
        handle |= (listener_t)entry;
        return handle;
    }
    
    listener_t Event::AddListener(event_t type, EntityComponent* component) {
        ListenerInfo* entry = LISTENER_TABLE[type].AddNew();
        entry->component = component;
        entry->type = ListenerInfo::LISTENER_COMPONENT;
        return EncodeListenerHandle(type, entry);
    }
    
    listener_t Event::AddListener(event_t type, Entity* entity) {
        ListenerInfo* entry = LISTENER_TABLE[type].AddNew();
        entry->entity = entity;
        entry->type = ListenerInfo::LISTENER_COMPONENT;
        return EncodeListenerHandle(type, entry);
    }
    
    listener_t Event::AddListener(event_t type, void (*function)(Event& event)) {
        ListenerInfo* entry = LISTENER_TABLE[type].AddNew();
        entry->function = function;
        entry->type = ListenerInfo::LISTENER_FUNCTION;
        return EncodeListenerHandle(type, entry);
    }
    
    /*Event::Listener* Event::AddListener(Event::Type type){
        auto new_listener = listeners.AddNew();
        dispatch_table[type].push_back(new_listener);
        new_listener->type = type;
        
        return new_listener;
    }*/

    /*void Event::RemoveListener(Event::Listener* listener){
        // you could probably deduct the type of listener from its pointer
        dispatch_table[listener->type].erase(std::find(dispatch_table[listener->type].begin(), dispatch_table[listener->type].end(), listener));
        listeners.Remove(listener);
    }*/
    
    void Event::RemoveListener(listener_t listener_id) {
        event_t type = listener_id >> 48;
        ListenerInfo* entry = (ListenerInfo*)((listener_id << 16) >> 16);
        LISTENER_TABLE[type].Remove(entry);
    }

    void Event::Dispatch(){
        Event* event = EVENT_QUEUE.GetFirstPtr();
        while (event){
            event_t type = event->type;

            for (auto& listener : LISTENER_TABLE[type]) {
                switch (listener.type) {
                    case ListenerInfo::LISTENER_COMPONENT:
                        listener.component->EventHandler(*event);
                        break;
                    case ListenerInfo::LISTENER_ENTITY:
                        std::cout << "NO HANDLERS FOR ENTITIES YET!!" << std::endl;
                        abort();
                        break;
                    case ListenerInfo::LISTENER_FUNCTION:
                        listener.function (*event);
                        break;
                }
            }

            EVENT_QUEUE.Remove();
            event = EVENT_QUEUE.GetFirstPtr();
        }
    }
    
    void Event::Post (const Event &event){
        *(EVENT_QUEUE.AddNew()) = event;
    }
}