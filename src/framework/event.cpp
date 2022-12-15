#include <framework/event.h>
#include <framework/entitycomponent.h>

namespace Core {
    Queue<Event> Core::Event::queue("event queue", 500);
    
    std::vector<std::vector<Event::Listener*>> Core::Event::dispatch_table(LAST_EVENT);
    Pool<Event::Listener> Core::Event::listeners("event listener pool", 50, false);
    
    Event::Listener* Event::AddListener(Event::Type type){
        auto new_listener = listeners.AddNew();
        dispatch_table[type].push_back(new_listener);
        new_listener->type = type;
        
        return new_listener;
    }

    void Event::RemoveListener(Event::Listener* listener){
        // you could probably deduct the type of listener from its pointer
        dispatch_table[listener->type].erase(std::find(dispatch_table[listener->type].begin(), dispatch_table[listener->type].end(), listener));
        listeners.Remove(listener);
    }

    void Event::Dispatch(){
        Event* event = Event::queue.GetFirstPtr();
        while (event){
            event_t type = event->type;

            // TODO: add the thing that check the distances
            for (auto listener : dispatch_table[type]){
                listener->ent->EventHandler(*event);
            }

            Event::queue.Remove();
            event = Event::queue.GetFirstPtr();
        }
    }
    
        void Event::Post (Event &event){
        *(Event::queue.AddNew()) = event;
    }
}