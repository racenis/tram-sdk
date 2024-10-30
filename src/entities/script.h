// TRAMWAY DRIFT AND DUNGEON EXPLORATION SIMULATOR 2022
// All rights reserved.

#ifndef TRAM_SDK_ENTITIES_SCRIPT_H
#define TRAM_SDK_ENTITIES_SCRIPT_H

#include <framework/entity.h>
#include <framework/serialization.h>

#include <framework/script.h>

#include <iostream>

namespace tram {

namespace ScriptableType {
    void Register(name_t base_type, Entity* (*constr_func)(name_t new_type, const SharedEntityData&, const ValueArray&), void (*destr_func)(Entity*));
    Entity* Make(name_t base_type, name_t type, const SharedEntityData&, const ValueArray&);
}

template <typename T>
class Scriptable : public T {
public:
    Scriptable(const SharedEntityData& shared_data, const ValueArray& field_array, name_t type) : T(shared_data, field_array) {
        this->type = type;
    }
    
    ~Scriptable() {}

    void UpdateParameters() {
        if (Script::CallFunction("__tram_impl_entity_update_parameters_callback", {this->id})) {
            T::UpdateParameters();
        }
    }
    
    void SetParameters() {
        if (Script::CallFunction("__tram_impl_entity_set_parameters_callback", {this->id})) {
            T::SetParameters();
        }
    }
    
    void Load() {
        std::cout << "LOADING FROM  C++" << std::endl;
        if (Script::CallFunction("__tram_impl_entity_load_callback", {this->id})) {
            T::Load();
        }
    }
    
    void Unload() {
        if (Script::CallFunction("__tram_impl_entity_unload_callback", {this->id})) {
            T::Unload();
        }
    }
    
    void Serialize() {
        /*if (Script::CallFunction("__tram_impl_entity_serialize_callback", {this->id})) {
            if (entity) entity->Serialize();
        }*/
    }

    inline void SanitizeData(Value& data) {
        // there might be some issues if the message's data pointer is not pointing
        // to a value, but rather something else.

        // hopefully we won't get segfaults.
        //auto type = data.GetType();
        bool valid_type = data.IsFloat() || data.IsInt() || data.IsBool() || data.GetType() == TYPE_NAME;

        if (!valid_type) data = Value();

        if (data.GetType() == TYPE_NAME) {
            if (!name_t::is_valid(data)) data = Value();
        }
    }

    void MessageHandler(Message& msg) {
        Value data = msg.data ? *msg.data_value : Value();

        SanitizeData(data);

        if (Script::CallFunction("__tram_impl_entity_message_handler_callback", {this->id, msg.type, msg.sender, msg.receiver, data})) {
            T::MessageHandler(msg);
        }
    }
    
    void EventHandler(Event& evt) {
        Value data = evt.data ? *evt.data_value : Value();

        SanitizeData(data);

        if (Script::CallFunction("__tram_impl_entity_event_handler_callback", {this->id, evt.type, evt.subtype, evt.poster_id, data})) {
            T::EventHandler(evt);
        }
    }
    
    name_t GetType() {
        return type;
    }
protected:
    name_t type;
};

}

#endif // TRAM_SDK_ENTITIES_SCRIPT_H