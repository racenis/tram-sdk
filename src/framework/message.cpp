// Tramway Drifting and Dungeon Exploration Simulator SDK Runtime

#include <framework/message.h>
#include <framework/entity.h>
#include <framework/logging.h>

#include <templates/queue.h>
#include <templates/pool.h>
#include <templates/stackpool.h>
#include <templates/hashmap.h>

#include <config.h>

#include <queue>

/**
 * @struct tram::Message framework/message.h
 * 
 * Message data.
 * 
 * Fill in the struct fields and then use Message::Send() to send it out to
 * listeners.
 * 
 * @property tram::Message::type
 * Type of the message.
 * 
 * @property tram::Message::sender
 * ID of the Entity that sent the message.
 * Either set to the ID of an Entity, or can be set to zero.
 * 
 * @property tram::Message::receiver
 * ID of the Entity that will receive the message.
 * Set to the ID of an Entity.
 * 
 * @property tram::Message::data
 * Pointer to arbitrary data.
 * Can be set to a nullptr. Some message types have specific data, e.g. structs
 * or other types that they need their data pointers to be pointing to.
 * For your own message types, you can use this pointer for whatever purpose you
 * want.
 * If uncertain, set this to a nullptr.
 * 
 * @property tram::Message::data_int
 * Arbitrary data integer.
 * 
 * @property tram::Message::data_value
 * Pointer to a value_t.
 * 
 * @see https://racenis.github.io/tram-sdk/documentation/framework/message.html
 */

/**
 * @typedef uint32_t message_t
 * Message type number.
 * Can be set to any of the values in Message::Type, or you can use
 * Message::Register() to allocate new message types.
 */

namespace tram {
    
static Queue<Message> message_queue("Message queue", MESSAGE_QUEUE_LIMIT);
static Pool<Message> message_pool("Message pool", MESSAGE_PRIORITY_QUEUE_LIMIT);
static StackPool<char> data_pool("Message data pool", MESSAGE_DATA_LIMIT);
static void(*intercept_callback)(const Message&) = nullptr;

static std::priority_queue<std::pair<double, Message*>, std::vector<std::pair<double, Message*>>, std::greater<std::pair<double, Message*>>> future_messages;

static Hashmap<message_t> name_t_to_message_t("name_t_to_message_t", (MESSAGE_TYPE_LIMIT*2)+11);

static const char* message_names[MESSAGE_TYPE_LIMIT] = {
    "none",
    "ping",
    "move-pick-up",
    "open",
    "close",
    "lock",
    "unlock",
    "toggle",
    "kill",
    "use",
    "start",
    "stop",
    "activate",
    "activate-once",
    "select",
    "set-progress",
    "set-animation",
    "set-flag-on",
    "set-flag-off"
};

static message_t last_type = Message::LAST_MESSAGE;

/// Registers a new message type.
/// @return Unique message type number.
message_t Message::Register(const char* name) {
    if (UID::is_empty(name)) {
        Log(Severity::CRITICAL_ERROR, System::CORE, "Message name '{}' is empty", name);
    }
    
    if (!UID::no_quote(name)) {
        Log(Severity::CRITICAL_ERROR, System::CORE, "Message name '{}' contains invalid characters", name);
    }
    
    for (event_t i = 0; i < last_type; i++) {
        if (strcmp(message_names[i], name) != 0) continue;
        
        Log(Severity::CRITICAL_ERROR, System::CORE, "Message name '{}' already in use", name);
    }
    
    if (last_type >= MESSAGE_TYPE_LIMIT) {
        Log(Severity::CRITICAL_ERROR, System::CORE, "Message count limit exceeded when registering '{}'", name);
    }
    
    message_names[last_type] = name;
    return last_type++;
}

/// Finds a message type from a name.
message_t Message::GetType(name_t name) {
    message_t type = name_t_to_message_t.Find(name);
    
    if (!type && name) {
        for (message_t i = 0; i < last_type; i++) {
            if (message_names[i] == name) {
                name_t_to_message_t.Insert(name, i);
                return i;
            }
        }
    }
    
    return type;
}

/// Gets a nessage type name.
name_t Message::GetName(message_t type) {
    if (type >= last_type) {
        Log(Severity::CRITICAL_ERROR, System::CORE, "Attempting index {} invalid message", type);
    }
    
    return message_names[type];
}

/// Returns the last message_t plus one.
/// Useful for iterating over all registered messages, i.e. in a 
/// `while (++message < Message::GetLast());` or a similar loop.
message_t Message::GetLast() {
    return last_type;
}

/// Dispatches sent messsages.
/// Dispatches the messages that have been sent using the Message::Send() function.
void Message::Dispatch() {
    auto now = GetTickTime();
    
    while (!future_messages.empty() && future_messages.top().first < now) {
        auto message = future_messages.top().second;
        auto receiver = Entity::Find(message->receiver);
        if (receiver) receiver->MessageHandler(*message);
        
        message_pool.Remove(message);
        future_messages.pop();
    }
    
    while (message_queue.size()) {
        Message& message = message_queue.front();
        Entity* receiver = Entity::Find(message.receiver);
        if (receiver) receiver->MessageHandler(message);
        if (intercept_callback) intercept_callback(message);

        message_queue.pop();
    }
    
    data_pool.Reset();
}

/// Sends a message.
/// Message will be delivered to the Entity with the ID number specified in the
/// Message::receiver field, by calling its Entity::MessageHandler() method.
void Message::Send(const Message& message) {
    if (!message.type || message.type >= last_type) {
        Log(Severity::WARNING, System::CORE, "Sent out an unregistered message type with {} index", message.type);
    }
    
    message_queue.push(message);
}

/// Semds a message into the future.
/// Same as Message::Send(), but the actual sending will be delayed until
/// specified ammount of ticks have passed.
/// @param message  Message to be sent.
/// @param when     How many ticks need to pass until the message will be sent
///                 out, i.e. if you want to send a message out in 1 secons,
///                 set this parameter to 60.
void Message::Send(const Message& message, float delay) {
    auto message_copy = message_pool.AddNew(message);
    auto abs_when = GetTickTime() + delay;
    
    if (!message.type || message.type >= last_type) {
        Log(Severity::WARNING, System::CORE, "Sent out an unregistered message type with {} index", message.type);
    }
    
    future_messages.push({abs_when, message_copy});
}

/// Allocates space for message data.
/// This allocation is useful for storing the additional data (Message::data pointer),
/// for messages, since all allocated space will be cleared once all messages have
/// been dispatched. This only applies to messages that are sent without a time delay,
/// if you're sending messages with a delay, then you'll need to allocate memory for
/// their data yourself.
/// @note Remember to only use POD data types.
void* Message::AllocateData(size_t ammount) {
    return data_pool.AddNew(ammount);
}

void Message::SetInterceptCallback(void(*callback)(const Message&)) {
    intercept_callback = callback;
}

}
