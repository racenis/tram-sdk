// TRAMWAY DRIFT AND DUNGEON EXPLORATION SIMULATOR 2022
// All rights reserved.

#include <framework/worldcell.h>

#include <entities/decoration.h>

#include <components/render.h>
#include <components/physics.h>
#include <components/animation.h>

#include <framework/serialization.h>
#include <framework/message.h>

namespace tram {

using namespace tram::Physics;

enum : uint32_t {
    FLAG_LOCKED = 1
};

enum {
    FIELD_FLAGS,
    FIELD_MODEL,
    FIELD_ANIMATION
};

static const uint32_t fields[3] = {
    TYPE_UINT32,
    TYPE_NAME,
    TYPE_NAME
}; 

void Decoration::Register() {
    Entity::RegisterType(
        "decoration", 
        [](const SharedEntityData& a, const ValueArray& b) -> Entity* { return new Decoration(a, b); },
        [](Entity* a) { delete a; },
        fields,
        3
    );
}

Decoration::Decoration(const SharedEntityData& shared_data, const ValueArray& field_array) : Entity(shared_data) {
    flags = field_array[FIELD_FLAGS];
    model = field_array[FIELD_MODEL];
    animation = field_array[FIELD_ANIMATION];
}

void Decoration::UpdateParameters() {
    if (!is_loaded) return;
    rendercomponent->SetLocation(location);
    rendercomponent->SetRotation(rotation);
    rendercomponent->SetWorldParameters(cell->HasInteriorLighting());
}

void Decoration::SetParameters() {
    if (!is_loaded) return;
    UpdateParameters();
    physicscomponent->SetLocation(location);
    physicscomponent->SetRotation(rotation);
}

void Decoration::Load(){
    rendercomponent.make();
    animationcomponent.make();
    physicscomponent.make();
    
    rendercomponent->SetParent(this);
    rendercomponent->SetModel(model);

    animationcomponent->SetParent(this);
    animationcomponent->SetModel(model);

    physicscomponent->SetParent(this);
    physicscomponent->SetCollisionGroup(COLL_DYNAMICOBJ);
    physicscomponent->SetModel(model);
    physicscomponent->SetMass(0.0f);
    
    rendercomponent->Init();
    animationcomponent->Init();
    physicscomponent->Init();
    
    rendercomponent->SetArmature(animationcomponent);
    
    if (animation) {
        std::cout << "PLAYING " << animation << std::endl;
        animationcomponent->PlayAnimation(animation, -1, 1.0f, 1.0f);
    }
    
    is_loaded = true;

    UpdateParameters();
}

void Decoration::Unload() {
    is_loaded = false;

    Serialize();

    rendercomponent.clear();
    animationcomponent.clear();
    physicscomponent.clear();
}

void Decoration::Serialize() {

}

void Decoration::MessageHandler(Message& msg) {
    switch (msg.type) {
        case Message::SELECT:
            if (flags & FLAG_LOCKED) return;
            Event::Post({
                .type = Event::SELECTED,
                .poster_id = this->id
            });
            break;
        case Message::ACTIVATE:
            break;
        case Message::ACTIVATE_ONCE:
            if (flags & FLAG_LOCKED) return;
            FireSignal(Signal::ACTIVATE);
            FireSignal(Signal::USE);
            break;
        case Message::LOCK:
            flags |= FLAG_LOCKED;
            break;
        case Message::UNLOCK:
            flags &= ~FLAG_LOCKED;
            break;
        case Message::TOGGLE:
            if (animation) {
                if (animationcomponent->IsPlayingAnimation(animation)) {
                    //animationcomponent->StopAnimation(animation);
                    animationcomponent->FadeAnimation(animation, false, 0.05f);
                } else {
                    animationcomponent->PlayAnimation(animation, -1, 1.0f, 1.0f);
                    animationcomponent->FadeAnimation(animation, true, 0.05f);
                }
            }
            break;
        case Message::KILL:
            delete this;
            break;
        case Message::START:
            if (animation) {
                animationcomponent->PlayAnimation(animation, -1, 1.0f, 1.0f);
                animationcomponent->FadeAnimation(animation, true, 0.05f);
            }
            break;
        case Message::STOP:
            if (animation) {
                //animationcomponent->StopAnimation(animation);
                animationcomponent->FadeAnimation(animation, false, 0.05f);
            }
            break;
        case Message::SET_ANIMATION:
            if (animation && animationcomponent->IsPlayingAnimation(animation)) {
                //animationcomponent->StopAnimation(animation);
                animationcomponent->FadeAnimation(animation, false, 0.05f);
                animation = *(Value*)msg.data_value;
                animationcomponent->PlayAnimation(animation, -1, 1.0f, 1.0f);
                animationcomponent->FadeAnimation(animation, true, 0.05f);
            } else {
                animation = *(Value*)msg.data_value;
            }
            break;
        default:
            std::cout << "Decoration " << name << " does not understand message " << Message::GetName(msg.type) << std::endl;  
    }
}

}
