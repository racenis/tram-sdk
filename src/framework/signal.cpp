// TRAMWAY DRIFT AND DUNGEON EXPLORATION SIMULATOR 2022
// All rights reserved.

#include <templates/hashmap.h>

#include <framework/signal.h>
#include <framework/entity.h>
#include <framework/value.h>

namespace tram {
const size_t MAX_SIGNAL_TYPES = 100;
    
static Hashmap<signal_t> name_t_to_signal_t("name_t_to_signal_t", (MAX_SIGNAL_TYPES*2)+11);

static const char* signal_names[MAX_SIGNAL_TYPES] = {
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
    signal_names[last_type] = name;
    return last_type++;
}

/// Finds a message type from a name.
signal_t Signal::GetType(name_t name) {
    signal_t type = name_t_to_signal_t.Find(name);
    
    if (!type) {
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
    return signal_names[type];
}


void SignalTable::Fire(signal_t signal, id_t sender) {
    //std::cout << "firing signal " << Signal::GetName(signal);
    
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
            Message::Send(msg, signals[i].delay * 60.0f);
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
            Message::Send(msg, signals[i].delay * 60.0f);
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