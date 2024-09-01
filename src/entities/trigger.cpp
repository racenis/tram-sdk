// TRAMWAY DRIFT AND DUNGEON EXPLORATION SIMULATOR 2022
// All rights reserved.

#include <framework/worldcell.h>

#include <entities/trigger.h>

#include <components/render.h>
#include <components/trigger.h>

#include <framework/serialization.h>
#include <framework/message.h>

namespace tram {

using namespace tram::Physics;

enum : uint32_t {
    TRIGGER_DISABLED = 1
};

enum {
    FIELD_FLAGS,
    FIELD_COLLISION_MASK,
    FIELD_MODEL,
};

static const uint32_t fields[3] = {
    TYPE_UINT32,
    TYPE_UINT32,
    TYPE_NAME
}; 

void Trigger::Register() {
    Entity::RegisterType(
        "trigger", 
        [](const SharedEntityData& a, const ValueArray& b) -> Entity* { return new Trigger(a, b); },
        [](Entity* a) { delete a; },
        fields,
        3
    );
}

name_t Trigger::GetType() {
    return "trigger";
}

Trigger::Trigger(const SharedEntityData& shared_data, const ValueArray& field_array) : Entity(shared_data) {
    model = field_array[FIELD_MODEL];
    trigger_flags = field_array[FIELD_FLAGS];
    collision_mask = field_array[FIELD_COLLISION_MASK];
}

void Trigger::UpdateParameters() {
    if (!is_loaded) return;
    //rendercomponent->SetLocation(location);
    //rendercomponent->SetRotation(rotation);
}

void Trigger::SetParameters() {
    if (!is_loaded) return;
    UpdateParameters();
    triggercomponent->SetLocation(location);
    triggercomponent->SetRotation(rotation);
}

void Trigger::Load(){
    //rendercomponent.make();
    triggercomponent.make();
    
    //rendercomponent->SetParent(this);
    //rendercomponent->SetModel(model);
    //rendercomponent->SetLightmap("fullbright");

    triggercomponent->SetParent(this);
    triggercomponent->SetCollisionMask(collision_mask);
    triggercomponent->SetCollisionGroup(COLL_TRIGGER);
    triggercomponent->SetModel(model);
    triggercomponent->SetActivationCallback([](TriggerComponent* comp, auto col) {
        dynamic_cast<Trigger*>(comp->GetParent())->Activate();
    });
    
    //rendercomponent->Init();
    triggercomponent->Init();
    
    is_loaded = true;

    SetParameters();
}

void Trigger::Unload() {
    is_loaded = false;

    Serialize();

    //rendercomponent.clear();
    triggercomponent.clear();
}

void Trigger::Serialize() {

}

void Trigger::Activate() {
    if (!(trigger_flags & TRIGGER_DISABLED)) {
        std::cout << "firing trigger " << name << std::endl;
        FireSignal(Signal::ACTIVATE);
    } else {
        std::cout << "trigger disabled " << name << std::endl; 
    }
}

void Trigger::MessageHandler(Message& msg) {
    switch (msg.type) {
        case Message::LOCK:
            trigger_flags |= TRIGGER_DISABLED;
            break;
        case Message::UNLOCK:
            trigger_flags &= ~TRIGGER_DISABLED;
            break;
        default:
            std::cout << "Trigger " << name << " does not understand message " << Message::GetName(msg.type) << std::endl;  
    }
}

}
