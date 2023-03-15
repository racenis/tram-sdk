// TRAMWAY DRIFT AND DUNGEON EXPLORATION SIMULATOR 2022
// All rights reserved.

#include <framework/message.h>
#include <framework/entity.h>

#include <templates/queue.h>
#include <templates/pool.h>

#include <queue>

namespace tram {
    Queue<Message> MESSAGE_QUEUE ("message queue", 500);
    Pool<Message> MESSAGE_POOL ("message pool", 250);
    
    std::priority_queue <std::pair<uint32_t, Message*>, std::vector<std::pair<uint32_t, Message*>>, std::greater<std::pair<uint32_t, Message*>>> FUTURE_MESSAGES;
    
    /// Dispatches the messages from the message queue.
    void Message::Dispatch() {
        auto now = GetTick();
        
        while (!FUTURE_MESSAGES.empty() && FUTURE_MESSAGES.top().first < now) {
            auto message = FUTURE_MESSAGES.top().second;
            auto receiver = Entity::FindByID(message->receiver);
            if (receiver) receiver->MessageHandler(*message);
            
            FUTURE_MESSAGES.pop();
        }
        
        while (Message* message = MESSAGE_QUEUE.GetFirstPtr()) {
            Entity* receiver = Entity::FindByID(message->receiver);
            if (receiver) receiver->MessageHandler(*message);

            MESSAGE_QUEUE.Remove();
        }
    }
    
    /// Adds a message to the message queue.
    void Message::Send(const Message &message) {
        *(MESSAGE_QUEUE.AddNew()) = message;
    }
    
    void Message::Send(const Message &message, uint32_t when) {
        auto message_copy = MESSAGE_POOL.AddNew(message);
        auto abs_when = GetTick() + when;
        
        FUTURE_MESSAGES.push({abs_when, message_copy});
    }
}
