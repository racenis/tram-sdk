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

enum {
    FIELD_MODEL,
    FIELD_ANIMATION
};

static const uint32_t fields[2] = {
    TYPE_NAME,
    TYPE_NAME
}; 

void Decoration::Register() {
    Entity::RegisterType(
        "decoration", 
        [](const SharedEntityData& a, const ValueArray& b) -> Entity* { return new Decoration(a, b); },
        [](Entity* a) { delete a; },
        fields,
        2
    );
}

Decoration::Decoration(const SharedEntityData& shared_data, const ValueArray& field_array) : Entity(shared_data) {
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
        animationcomponent->PlayAnimation(animation, -1, 1.0f, 1.0f);
        std::cout << "PLAYING" << std::endl;
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
            Event::Post({
                .type = Event::SELECTED,
                .poster_id = this->id
            });
            break;
        case Message::ACTIVATE:
            break;
        case Message::ACTIVATE_ONCE:
            FireSignal(Signal::ACTIVATE);
            FireSignal(Signal::USE);
            break;
        case Message::TOGGLE:
            if (animation) {
                if (animationcomponent->IsPlayingAnimation(animation)) {
                    animationcomponent->StopAnimation(animation);
                } else {
                    animationcomponent->PlayAnimation(animation, -1, 1.0f, 1.0f);
                }
            }
            break;
        case Message::KILL:
            delete this;
            break;
        case Message::START:
            if (animation) {
                animationcomponent->PlayAnimation(animation, -1, 1.0f, 1.0f);
            }
            break;
        case Message::STOP:
            if (animation) {
                animationcomponent->StopAnimation(animation);
            }
            break;
        case Message::SET_ANIMATION:
            if (animation && animationcomponent->IsPlayingAnimation(animation)) {
                animationcomponent->StopAnimation(animation);
                animation = *(Value*)msg.data_value;
                animationcomponent->PlayAnimation(animation, -1, 1.0f, 1.0f); 
            } else {
                animation = *(Value*)msg.data_value;
            }
            break;
        default:
            std::cout << "Decoration " << name << " does not understand message " << Message::GetName(msg.type) << std::endl;  
    }
}

}
