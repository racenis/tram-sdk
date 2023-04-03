// TRAMWAY DRIFT AND DUNGEON EXPLORATION SIMULATOR 2022
// All rights reserved.

#include <framework/message.h>
#include <framework/entity.h>

#include <templates/queue.h>
#include <templates/pool.h>
#include <templates/stackpool.h>

#include <queue>

namespace tram {

static Queue<Message> message_queue ("message queue", 500);
static Pool<Message> message_pool ("message pool", 250);
static StackPool<char> data_pool ("message data pool", 2000);

std::priority_queue <std::pair<uint32_t, Message*>, std::vector<std::pair<uint32_t, Message*>>, std::greater<std::pair<uint32_t, Message*>>> future_messages;

/// Registers a new message type.
/// @return Unique message type number.
message_t Message::Register() {
    static message_t last_type = LAST_MESSAGE;
    
    return last_type++;
}

/// Dispatches sent messsages.
/// Dispatches the messages that have been sent using the Message::Send() function.
void Message::Dispatch() {
    auto now = GetTick();
    
    while (!future_messages.empty() && future_messages.top().first < now) {
        auto message = future_messages.top().second;
        auto receiver = Entity::Find(message->receiver);
        if (receiver) receiver->MessageHandler(*message);
        
        future_messages.pop();
    }
    
    while (message_queue.size()) {
        Message& message = message_queue.front();
        Entity* receiver = Entity::Find(message.receiver);
        if (receiver) receiver->MessageHandler(message);

        message_queue.pop();
    }
    
    data_pool.Reset();
}

/// Sends a message.
/// Message will be delivered to the Entity with the ID number specified in the
/// Message::receiver field, by calling its Entity::MessageHandler() method.
void Message::Send (const Message& message) {
    message_queue.push(message);
}

/// Semds a message into the future.
/// Same as Message::Send(), but the actual sending will be delayed until
/// specified ammount of ticks have passed.
/// @param message  Message to be sent.
/// @param when     How many ticks need to pass until the message will be sent
///                 out, i.e. if you want to send a message out in 1 secons,
///                 set this parameter to 60.
void Message::Send (const Message& message, uint32_t when) {
    auto message_copy = message_pool.AddNew(message);
    auto abs_when = GetTick() + when;
    
    future_messages.push({abs_when, message_copy});
}

/// Allocates space for message data.
/// This allocation is useful for storing the additional data (Message::data pointer),
/// for messages, since all allocated space will be cleared once all messages have
/// been dispatched. This only applies to messages that are sent without a time delay,
/// if you're sending messages with a delay, then you'll need to allocate memory for
/// their data yourself.
/// @note Remember to only use POD data types.
void* Message::AllocateData (size_t ammount) {
    return data_pool.AddNew(ammount);
}

}
