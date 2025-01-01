// Tramway Drifting and Dungeon Exploration Simulator SDK Runtime

#include <templates/hashmap.h>

#include <framework/signal.h>
#include <framework/entity.h>
#include <framework/value.h>
#include <framework/logging.h>

#include <config.h>

/**
 * @struct tram::Signal framework/signal.h <framework/signal.h>
 * 
 * Message template.
 * 
 * Table of Message templates that can be sent off by entities on certain triggers. 
 * 
 * @see https://racenis.github.io/tram-sdk/documentation/framework/signal.html
 */

/**
 * @class tram::SignalTable framework/signal.h <framework/signal.h>
 * 
 * List of Message templates.
 * 
 * Table of Message templates, i.e. Signal structs, that can be sent off by
 * entities on certain triggers.
 */

namespace tram {

static Hashmap<signal_t> name_t_to_signal_t("name_t_to_signal_t", SIGNAL_TYPE_LIMIT);

static const char* signal_names[SIGNAL_TYPE_LIMIT] = {
    "none",
    "spawn",
    "kill",
    "activate",
    "use",
    "locked-use",
    "open",
    "close",
    "end-open",
    "end-close",
    "trigger",
    "enter-trigger",
    "exit-trigger",
    "progress"
};

static signal_t last_type = Signal::LAST_SIGNAL;

/// Registers a new message type.
/// @return Unique message type number.
signal_t Signal::Register(const char* name) {
    if (UID::is_empty(name)) {
        Log(Severity::CRITICAL_ERROR, System::CORE, "Signal name '{}' is empty", name);
    }
    
    if (!UID::no_quote(name)) {
        Log(Severity::CRITICAL_ERROR, System::CORE, "Signal name '{}' contains invalid characters", name);
    }
    
    for (event_t i = 0; i < last_type; i++) {
        if (strcmp(signal_names[i], name) != 0) continue;
        
        Log(Severity::CRITICAL_ERROR, System::CORE, "Signal name '{}' already in use", name);
    }
    
    if (last_type >= SIGNAL_TYPE_LIMIT) {
        Log(Severity::CRITICAL_ERROR, System::CORE, "Signal count limit exceeded when registering '{}'", name);
    }
    
    signal_names[last_type] = name;
    return last_type++;
}

/// Finds a message type from a name.
signal_t Signal::GetType(name_t name) {
    signal_t type = name_t_to_signal_t.Find(name);
    
    if (!type && name) {
        for (signal_t i = 0; i < last_type; i++) {
            if (signal_names[i] == name) {
                name_t_to_signal_t.Insert(name, i);
                return i;
            }
        }
    }
    
    return type;
}

/// Gets a nessage type name.
name_t Signal::GetName(signal_t type) {
    if (type >= SIGNAL_TYPE_LIMIT) {
        Log(Severity::CRITICAL_ERROR, System::CORE, "Attempting index {} invalid signal", type);
    }
    
    return signal_names[type];
}


void SignalTable::Fire(signal_t signal, id_t sender) {
    for (size_t i = 0; i < signal_count; i++) {
        if (signals[i].type != signal || signals[i].limit == 0) continue;
        
        Message msg;
        
        msg.type = signals[i].message_type;
        msg.receiver = Entity::Find(signals[i].receiver)->GetID(); // this might crasherinoo
        // TODO: make Entity::Find not crash; erore message /// ahhh who cares
        msg.data = signals[i].data;
        msg.sender = sender;
        
        if (signals[i].data) {
            *msg.data_value = *(Value*)signals[i].data;
        } else {
            msg.data_value = nullptr;
        }
        
        if (signals[i].delay != 0.0f) {
            Message::Send(msg, signals[i].delay);
        } else {
            Message::Send(msg);
        }
    
        if (signals[i].limit != -1) signals[i].limit--;
    }
}

void SignalTable::Fire(signal_t signal, id_t sender, Value value) {
    for (size_t i = 0; i < signal_count; i++) {
        if (signals[i].type != signal || signals[i].limit == 0) continue;
        
        Message msg;
        
        msg.type = signals[i].message_type;
        msg.receiver = Entity::Find(signals[i].receiver)->GetID();
        msg.data = signals[i].data;
        msg.sender = sender;
        msg.data_value = (Value*)Message::AllocateData(sizeof(Value));
        
        // send override if set, otherwise pass on provided data
        if (signals[i].data) {
            *msg.data_value = *(Value*)signals[i].data;
        } else {
            *msg.data_value = value;
        }
        
        
        if (signals[i].delay != 0.0f) {
            Message::Send(msg, signals[i].delay);
        } else {
            Message::Send(msg);
        }
    
        if (signals[i].limit != -1) signals[i].limit--;
    }
}

void SignalTable::Add(const Signal& signal) {
    //assert(signal_count < 10);
    if (signal_count >= 10) return; // TODO: add errore message
    signals[signal_count++] = signal;
}

}