// Tramway Drifting and Dungeon Exploration Simulator SDK Runtime

#include <extensions/kitchensink/entities.h>

#include <extensions/kitchensink/soundtable.h>

#include <components/render.h>
#include <components/physics.h>

#include <templates/hashmap.h>

namespace tram::Ext::Kitchensink {

// hacky parent/child transform thing.
// it WILL crash if any of the entities gets killed, btw
struct EntityChildren {
    std::vector<id_t> children;
};

Hashmap<EntityChildren> hierarchy_map("hierarchy_map", 200);

void AddChild(name_t parent, id_t child) {
    auto parent_id = Entity::Find(parent)->GetID();
    auto hierarchy = hierarchy_map.Find(parent_id);
    hierarchy.children.push_back(child);
    hierarchy_map.Insert(parent_id, hierarchy);
}

void UpdateHierarchy(id_t parent_id) {
    auto hierarchy = hierarchy_map.Find(parent_id);
    for (id_t child : hierarchy.children) {
        Entity::Find(child)->UpdateParameters();
    }
}

/******************************************************************************/
/*                                                                            */
/*                             FUNC_BUTTON                                    */
/*                                                                            */
/******************************************************************************/

/**
 * @class tram::Ext::Kitchensink::Button extensions/kitchensink/entities.h <extensions/kitchensink/entities.h>
 * 
 * Essentially a func_door.
 * 
 * It can open. It can close. It responds to Signal inputs and gives out
 * outputs. It can be a door. It can be a lift. It can even be a button.
 */

// this macro could be converted into a method
#define PlaySound(TYPE) if (sound) SoundTable::Find(sound)->PlaySound(TYPE, origin, 1.0f);

enum {
    BUTTON_FIELD_FLAGS,
    BUTTON_FIELD_MODEL,
    BUTTON_FIELD_LIGHTMAP,
    BUTTON_FIELD_ORIGIN,
    BUTTON_FIELD_DIRECTION,
    BUTTON_FIELD_SPEED,
    BUTTON_FIELD_PAUSE,
    BUTTON_FIELD_DISTANCE,
    BUTTON_FIELD_SOUND,
    BUTTON_FIELD_PARENT,
    
    BUTTON_FIELD_LAST
};

// speed, pause, distance,  parent, sound,

static const Type button_fields[BUTTON_FIELD_LAST] = {
    TYPE_UINT32,
    TYPE_NAME,
    TYPE_NAME,
    TYPE_VEC3,
    TYPE_VEC3,
    TYPE_FLOAT32,
    TYPE_FLOAT32,
    TYPE_FLOAT32,
    TYPE_NAME,
    TYPE_NAME
};

enum {
    BUTTON_FLAG_ROTARY = 1,     // button that rotates
    BUTTON_FLAG_MOMENTARY = 2,  // button that automatically unbuttons
    BUTTON_FLAG_TOGGLE = 4,     
    BUTTON_FLAG_LOCKED = 8,
    BUTTON_FLAG_PUSHED = 16,
};

enum {
    BUTTON_STATE_NADIR_WAITING,
    BUTTON_STATE_NADIR_READY,
    BUTTON_STATE_RISING,
    BUTTON_STATE_ZENITH_WAITING,
    BUTTON_STATE_ZENITH_READY,
    BUTTON_STATE_LOWERING
};

void Button::Register() {
    Entity::RegisterType(
        "button", 
        [](const SharedEntityData& a, const ValueArray& b) -> Entity* { return new Button(a, b); },
        [](Entity* a) { delete a; },
        button_fields,
        BUTTON_FIELD_LAST
    );
}

name_t Button::GetType() {
    return "button";
}

Button::Button(const SharedEntityData& shared_data, const ValueArray& field_array) : Entity(shared_data) {
    flags =         field_array[BUTTON_FIELD_FLAGS];
    model =         field_array[BUTTON_FIELD_MODEL];
    lightmap =     field_array[BUTTON_FIELD_LIGHTMAP];
    origin =        field_array[BUTTON_FIELD_ORIGIN];
    direction =     glm::normalize((vec3)field_array[BUTTON_FIELD_DIRECTION]);
    speed =         field_array[BUTTON_FIELD_SPEED];
    pause =         field_array[BUTTON_FIELD_PAUSE];
    distance =      field_array[BUTTON_FIELD_DISTANCE];
    sound =         field_array[BUTTON_FIELD_SOUND];
    parent =        field_array[BUTTON_FIELD_PARENT];
    
    // this tracks the progress of the button. it starts at 0.0, which is the
    // button at its resting state. it can be increased up to 1.0, which is the
    // button at its pressed state.
    progress = 0.0f;
    
    // timer is used to track the pause of a button. the button pauses when it
    // reaches its most pressed or resting state, so that buttons with a very
    // rapid progress rate don't get accidentally pressed repeatedly
    timer = 0.0f;
    
    // this tracks the state of the button.
    // in future user might be able to change the default value
    state = BUTTON_STATE_NADIR_READY; 
    
    // the progress is applied to the starting position and rotation
    start_pos = location;
    start_rot = rotation;
    
    // this tracks the update cycles since the button was last updated
    last_activate = 0;
}

void Button::UpdateParameters() {
    if (!IsLoaded()) return;
    
    // first we apply the progress to the position or rotation of the entity    
    vec3 pos = start_pos;
    quat rot = start_rot;

    if (flags & BUTTON_FLAG_ROTARY) {
        rot = glm::rotate(rot, progress * distance, direction);
        
        pos -= origin;
        pos = rot * pos;
        pos += origin;
    } else {
        pos += direction * distance * progress;
    }

    // then check if parent has moved
    if (parent) {
        // if parent doesn't start at origin, this will glitch out.
        // TODO: fix
        pos += Entity::Find(parent)->GetLocation();
    }
    
    location = pos;
    rotation = rot;
    
    // then we update the render and physics components
    rendercomponent->SetLocation(pos);
    rendercomponent->SetRotation(rot);
    physicscomponent->SetLocation(pos);
    physicscomponent->SetRotation(rot);
    
    // and update entities that are parented to this one
    UpdateHierarchy(id);
}

void Button::SetParameters() {
    if (!IsLoaded()) return;
    UpdateParameters();
}

void Button::Load(){
    rendercomponent.make();
    physicscomponent.make();
    
    rendercomponent->SetParent(this);
    rendercomponent->SetModel(model);
    rendercomponent->SetLightmap(lightmap);

    physicscomponent->SetParent(this);
    physicscomponent->SetCollisionGroup(Physics::COLL_STATICOBJ);
    physicscomponent->SetCollisionMask(-1 ^ (Physics::COLL_WORLDOBJ | Physics::COLL_STATICOBJ));
    physicscomponent->SetModel(model);
    physicscomponent->SetMass(0.0f);
    physicscomponent->SetUpdateParentTransform(false);
    
    rendercomponent->Init();
    physicscomponent->Init();
    
    Entity::flags |= LOADED;

    UpdateParameters();
}

void Button::Unload() {
    Entity::flags &= ~LOADED;

    Serialize();

    rendercomponent.clear();
    physicscomponent.clear();
}

void Button::Serialize() {

}



void Button::EventHandler(Event &event) {
    if (event.type != Event::TICK) return;
    
    switch (state) {   
     
        case BUTTON_STATE_NADIR_WAITING:
            timer += 1.0f / (pause * 60.0f);
            if (timer >= 1.0f) {
                timer = 0.0f;
                state = BUTTON_STATE_NADIR_READY;
            }
            
            break;
            
        case BUTTON_STATE_NADIR_READY:
            tick.clear();
            break;
            
        case BUTTON_STATE_RISING:
            progress += 1.0f / (speed * 60.0f);
            FireSignal(Signal::PROGRESS, progress);
            if (progress >= 1.0f) {
                progress = 1.0f;
                state = BUTTON_STATE_ZENITH_WAITING;
                PlaySound(SOUND_END_OPEN)
                FireSignal(Signal::END_OPEN);
            }
            if (last_activate > 10 && (flags & BUTTON_FLAG_MOMENTARY)) {
                state = BUTTON_STATE_ZENITH_WAITING;
                PlaySound(SOUND_END_OPEN)
                FireSignal(Signal::END_OPEN);
            }
            
            break;
            
        case BUTTON_STATE_ZENITH_WAITING:
            timer += 1.0f / (pause * 60.0f);
            if (timer >= 1.0f) {
                timer = 0.0f;
                state = BUTTON_STATE_ZENITH_READY;
            }
            break;
            
        case BUTTON_STATE_ZENITH_READY:
            if (flags & BUTTON_FLAG_TOGGLE) {
                tick.clear();
            } else {
                state = BUTTON_STATE_LOWERING;
                PlaySound(SOUND_CLOSE)
                FireSignal(Signal::CLOSE);
            }
            break;
            
        case BUTTON_STATE_LOWERING:
            progress -= 1.0f / (speed * 60.0f);
            FireSignal(Signal::PROGRESS, progress);
            if (progress <= 0.0f) {
                progress = 0.0f;
                state = BUTTON_STATE_NADIR_WAITING;
                PlaySound(SOUND_END_CLOSE)
                FireSignal(Signal::END_CLOSE);
            }
            if (last_activate > 10 && (flags & BUTTON_FLAG_MOMENTARY) && (flags & BUTTON_FLAG_TOGGLE)) {
                state = BUTTON_STATE_NADIR_WAITING;
                PlaySound(SOUND_END_CLOSE)
                FireSignal(Signal::END_CLOSE);
            }
            break;
            
    }
    
    UpdateParameters();
    last_activate++;
}

void Button::MessageHandler(Message& msg) {
    if (msg.type == Message::SELECT && !(flags & BUTTON_FLAG_LOCKED) && (state == BUTTON_STATE_NADIR_READY || state == BUTTON_STATE_ZENITH_READY)) {
        Event::Post({
            .type = Event::SELECTED,
            .poster = this->id
        });
    }
    
    // button is pressed and it is not momentary
    if (msg.type == Message::ACTIVATE_ONCE /*&& !(flags & BUTTON_FLAG_MOMENTARY)*/ && !(flags & BUTTON_FLAG_LOCKED)) {
        switch (state) {
            case BUTTON_STATE_NADIR_READY:  tick.make(Event::TICK, this); state = BUTTON_STATE_RISING;   PlaySound(SOUND_OPEN)   FireSignal(Signal::OPEN);   break;
            case BUTTON_STATE_ZENITH_READY: tick.make(Event::TICK, this); state = BUTTON_STATE_LOWERING; PlaySound(SOUND_CLOSE)  FireSignal(Signal::CLOSE);  break;
            default:                                                                                                break;
        }
        
        last_activate = 0;
    }
    
    // play locked sound
    if (msg.type == Message::ACTIVATE_ONCE && (flags & BUTTON_FLAG_LOCKED)) {
        PlaySound(SOUND_LOCKED)
        FireSignal(Signal::LOCKED_USE, progress);
    }
    
    // button is being pressed and it is momentary
    if (msg.type == Message::ACTIVATE && flags & BUTTON_FLAG_MOMENTARY && !(flags & BUTTON_FLAG_LOCKED)) {
        last_activate = 0;
    }
    
    if (msg.type == Message::SET_PROGRESS) {
        progress = *msg.data_value;
        UpdateParameters();
    }
    
    // opening and closing
    if ((msg.type == Message::OPEN || msg.type == Message::TOGGLE) && state == BUTTON_STATE_NADIR_READY) {
        PlaySound(SOUND_OPEN)
        state = BUTTON_STATE_RISING;
        tick.make(Event::TICK, this);
    }
    if ((msg.type == Message::CLOSE || msg.type == Message::TOGGLE) && state == BUTTON_STATE_ZENITH_READY) {
        PlaySound(SOUND_CLOSE)
        state = BUTTON_STATE_LOWERING;
        tick.make(Event::TICK, this);
    }
    
    
    // locking and unlocking
    if (msg.type == Message::LOCK) {
        flags |= BUTTON_FLAG_LOCKED;
    } 
    
    if (msg.type == Message::UNLOCK) {
        flags &= ~BUTTON_FLAG_LOCKED;
    } 
}

}