// Tramway Drifting and Dungeon Exploration Simulator SDK Runtime

#include <framework/worldcell.h>

#include <entities/trigger.h>

#include <components/render.h>
#include <components/trigger.h>

#include <framework/serialization.h>
#include <framework/message.h>

/**
 * @class tram::Trigger entities/trigger.h <entities/trigger.h>
 * 
 * TriggerComponent wrapper.
 * 
 * @see https://racenis.github.io/tram-sdk/documentation/entities/trigger.html
 */

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

static const Type fields[3] = {
    TYPE_UINT32,
    TYPE_UINT32,
    TYPE_NAME
}; 

static bool draw_trigger = false;

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
    if (!IsLoaded()) return;
    
    if (rendercomponent) {
        rendercomponent->SetLocation(location);
        rendercomponent->SetRotation(rotation);
    }
}

void Trigger::SetParameters() {
    if (!IsLoaded()) return;
    UpdateParameters();
    triggercomponent->SetLocation(location);
    triggercomponent->SetRotation(rotation);
}

void Trigger::Load(){    
    SetupModel();

    triggercomponent.make();
    triggercomponent->SetParent(this);
    triggercomponent->SetCollisionMask(collision_mask);
    triggercomponent->SetCollisionGroup(COLL_TRIGGER);
    triggercomponent->SetModel(model);
    triggercomponent->SetActivationCallback([](TriggerComponent* comp, auto col) {
        dynamic_cast<Trigger*>(comp->GetParent())->Activate();
    });
    
    
    triggercomponent->Init();
    
    flags |= LOADED;

    SetParameters();
}

void Trigger::Unload() {
    flags &= ~LOADED;

    Serialize();

    rendercomponent.clear();
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

void Trigger::SetupModel() {
    if (!draw_trigger) return;
    
    rendercomponent.make();
    rendercomponent->SetParent(this);
    rendercomponent->SetModel(model);
    rendercomponent->SetLightmap("fullbright");
    rendercomponent->Init();
}


bool Trigger::Trigger::IsDrawTrigger() {
    return draw_trigger;
}

void Trigger::SetDrawTrigger(bool draw) {
    draw_trigger = draw;
    
    // TODO: add code to notify every trigger about this
}

}
