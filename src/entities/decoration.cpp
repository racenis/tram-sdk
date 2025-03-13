// Tramway Drifting and Dungeon Exploration Simulator SDK Runtime

#include <framework/worldcell.h>

#include <entities/decoration.h>

#include <render/light.h>

#include <components/render.h>
#include <components/physics.h>
#include <components/animation.h>

#include <framework/serialization.h>
#include <framework/message.h>

/**
 * @class tram::Decoration entities/decoration.h <entities/decoration.h>
 * 
 * 3D Model with an AnimationComponent attached to it.
 * @see https://racenis.github.io/tram-sdk/documentation/entities/decoration.html
 */

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

void Decoration::Register() {
    Entity::RegisterType(
        "decoration", 
        [](const SharedEntityData& a, const ValueArray& b) -> Entity* { return new Decoration(a, b); },
        [](Entity* a) { delete a; },
        {
            {FIELD_FLAGS,       TYPE_UINT32,    FIELD_SERIALIZE},
            {FIELD_MODEL,       TYPE_NAME,      FIELD_SERIALIZE},
            {FIELD_ANIMATION,   TYPE_NAME,      FIELD_SERIALIZE}
        }
    );
}

name_t Decoration::GetType() {
    return "decoration";
}

Decoration::Decoration(const SharedEntityData& shared_data, const ValueArray& field_array) : Entity(shared_data) {
    decoration_flags = field_array[FIELD_FLAGS];
    model = field_array[FIELD_MODEL];
    animation = field_array[FIELD_ANIMATION];
}

void Decoration::UpdateParameters() {
    if (!IsLoaded()) return;
    rendercomponent->SetLocation(location);
    rendercomponent->SetRotation(rotation);
    rendercomponent->SetDirectionaLight(!cell->HasInteriorLighting());
    
    rendercomponent->SetEnvironmentMap(Render::LightGraph::LookupEnvironmentMap(location));
}

void Decoration::SetParameters() {
    if (!IsLoaded()) return;
    UpdateParameters();
    physicscomponent->SetLocation(location);
    physicscomponent->SetRotation(rotation);
}

void Decoration::Load() {
    rendercomponent.make();
    physicscomponent.make();
    
    rendercomponent->SetParent(this);
    rendercomponent->SetModel(model);

    physicscomponent->SetParent(this);
    physicscomponent->SetCollisionGroup(COLL_STATICOBJ);
    physicscomponent->SetCollisionMask(-1 ^ (COLL_WORLDOBJ | COLL_STATICOBJ));
    physicscomponent->SetModel(model);
    physicscomponent->SetMass(0.0f);
    
    rendercomponent->Init();
    
    physicscomponent->Init();
    
    
    
    if (animation) {
        RequestAnimationComponent();
        
        animationcomponent->Play(animation, -1, 1.0f, 1.0f);
    }
    
    flags |= LOADED;

    UpdateParameters();
}

void Decoration::RequestAnimationComponent() {
    if (animationcomponent) return;
    
    animationcomponent.make();
    animationcomponent->SetParent(this);
    animationcomponent->SetModel(model);
    animationcomponent->Init();
    
    rendercomponent->SetArmature(animationcomponent);
}

void Decoration::Unload() {
    flags &= ~LOADED;

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
            if (decoration_flags & FLAG_LOCKED) return;
            
            Event::Post({
                .type = Event::SELECTED,
                .poster = this->id
            });
            
            break;
        case Message::ACTIVATE:
            break;
        case Message::ACTIVATE_ONCE:
            if (decoration_flags & FLAG_LOCKED) return;
            
            FireSignal(Signal::ACTIVATE);
            FireSignal(Signal::USE);
            
            break;
        case Message::LOCK:
            decoration_flags |= FLAG_LOCKED;
            break;
        case Message::UNLOCK:
            decoration_flags &= ~FLAG_LOCKED;
            break;
        case Message::TOGGLE:
            if (animation) {
                RequestAnimationComponent();
                if (animationcomponent->IsPlaying(animation)) {
                    //animationcomponent->StopAnimation(animation);
                    //animationcomponent->FadeAnimation(animation, false, 0.05f);
                    animationcomponent->FadeOut(animation, 1.0f);
                } else {
                    animationcomponent->Play(animation, -1, 1.0f, 1.0f);
                    animationcomponent->FadeIn(animation, 1.0f);
                }
            }
            break;
        case Message::KILL:
            Yeet();
            
            break;
        case Message::START:
            if (animation) {
                RequestAnimationComponent();
                animationcomponent->Play(animation, -1, 1.0f, 1.0f);
                animationcomponent->FadeIn(animation, 1.0f);
            }
            break;
        case Message::STOP:
            if (animation) {
                RequestAnimationComponent();
                animationcomponent->FadeOut(animation, 1.0f);
            }
            break;
        case Message::SET_ANIMATION:
            if (!msg.data_value) {
                Log(Severity::WARNING, System::MISC, "Decoration {} received a SET_ANIMATION without a parameter", name);
                return;
            }
            if (msg.data_value->GetType() != TYPE_NAME) {
                Log(Severity::WARNING, System::MISC, "Decoration {} received a SET_ANIMATION with a non-name parameter", name);
                return;
            }
        
            if (animation && animationcomponent->IsPlaying(animation)) {
                animationcomponent->FadeOut(animation, 1.0f);
                animation = *msg.data_value;
                animationcomponent->Play(animation, -1, 1.0f, 1.0f);
                animationcomponent->FadeIn(animation, 1.0f);
            } else {
                animation = *msg.data_value;
            }
            break;
        default:
            Log(Severity::WARNING, System::MISC, "Decoration {} does not understand message {}", name, Message::GetName(msg.type)); 
    }
}

}
