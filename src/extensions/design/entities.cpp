// TRAMWAY DRIFT AND DUNGEON EXPLORATION SIMULATOR 2022
// All rights reserved.

#include <extensions/design/entities.h>

#include <components/render.h>
#include <components/physics.h>

namespace tram::Ext::Design {

/******************************************************************************/
/*                                                                            */
/*                             FUNC_BUTTON                                    */
/*                                                                            */
/******************************************************************************/
    
enum {
    BUTTON_FIELD_FLAGS,
    BUTTON_FIELD_MODEL,
    BUTTON_FIELD_COLLMODEL,
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

static const uint32_t button_fields[BUTTON_FIELD_LAST] = {
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
    BUTTON_STATE_WAITING,
    BUTTON_STATE_PUSHED,
    BUTTON_STATE_APEXED,
    BUTTON_STATE_RELEASED,
    BUTTON_STATE_REPUSHED
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

Button::Button(const SharedEntityData& shared_data, const ValueArray& field_array) : Entity(shared_data) {
    flags =         field_array[BUTTON_FIELD_FLAGS];
    model =         field_array[BUTTON_FIELD_MODEL];
    collmodel =     field_array[BUTTON_FIELD_COLLMODEL];
    origin =        field_array[BUTTON_FIELD_ORIGIN];
    direction =     glm::normalize((vec3)field_array[BUTTON_FIELD_DIRECTION]);
    speed =         field_array[BUTTON_FIELD_SPEED];
    pause =         field_array[BUTTON_FIELD_PAUSE];
    distance =      field_array[BUTTON_FIELD_DISTANCE];
    sound =         field_array[BUTTON_FIELD_SOUND];
    parent =        field_array[BUTTON_FIELD_PARENT];
    
    progress = 0.0f;
    wait = 0.0f;
    state = BUTTON_STATE_WAITING;
    
    //std::cout << origin.x << " " << origin.y << " " << origin.z << " " << std::endl;
    //std::cout << direction.x << " " << direction.y << " " << direction.z << " " << std::endl;
    //std::cout << speed << " " << pause << " " << distance << " " << std::endl;
}

void Button::UpdateParameters() {
    if (!is_loaded) return;
    
    vec3 pos = location;
    quat rot = rotation;
    
    if (flags & BUTTON_FLAG_ROTARY) {
        rot = glm::rotate(rot, progress * distance, direction);
        
        pos -= origin;
        pos = rot * pos;
        pos += origin;
    } else {
        pos += direction * distance * progress;
    }
    
    rendercomponent->SetLocation(pos);
    rendercomponent->SetRotation(rot);
    physicscomponent->SetLocation(pos);
    physicscomponent->SetRotation(rot);

}

void Button::SetParameters() {
    if (!is_loaded) return;
    UpdateParameters();
}

void Button::Load(){
    rendercomponent.make();
    physicscomponent.make();
    
    rendercomponent->SetParent(this);
    rendercomponent->SetModel(model);

    physicscomponent->SetParent(this);
    physicscomponent->SetCollisionGroup(Physics::COLL_DYNAMICOBJ);
    physicscomponent->SetModel(collmodel);
    physicscomponent->SetMass(0.0f);
    physicscomponent->SetUpdateParentTransform(false);
    
    rendercomponent->Init();
    physicscomponent->Init();
    is_loaded = true;

    UpdateParameters();
}

void Button::Unload() {
    is_loaded = false;

    Serialize();

    rendercomponent.clear();
    physicscomponent.clear();
}

void Button::Serialize() {

}



void Button::Update() {
    
    // special momentary options
    if (flags & BUTTON_FLAG_MOMENTARY) {
        
        // if button is let go
        if (state == BUTTON_STATE_PUSHED) {
            if (flags & BUTTON_FLAG_TOGGLE) {
                //speed = -speed;
                SwitchState(BUTTON_STATE_WAITING);
            } else {
                SwitchState(BUTTON_STATE_APEXED);
            }
        }
        
        // if button is still being pushed
        if (state == BUTTON_STATE_REPUSHED) {
            state = BUTTON_STATE_PUSHED;
            wait = 0.0f;
        }
    }
    
    // this will make the button progress forward
    if (state == BUTTON_STATE_PUSHED) {
        progress += 1.0f / (speed * 60.0f);
        
        if (progress >= 1.0f) {
            progress = 1.0f;
            SwitchState(BUTTON_STATE_APEXED);
        }
        
        // we allow only momentary toggle buttons to be pushed backwards
        if (progress < 0.0f && flags & BUTTON_FLAG_MOMENTARY) {
            progress = 0.0f;
            //speed = -speed;
            SwitchState(BUTTON_STATE_APEXED);
        }
        
        // momentary buttons need to fire every time they move
        if (flags & BUTTON_FLAG_MOMENTARY) {
            if (speed > 0.0f) {
                FireSignal(Signal::OPEN);
            } else {
                FireSignal(Signal::CLOSE);
            }
        }
        
        UpdateParameters();
    }
    
    // button is at apex, regular case
    // this will make the button wait a little bit at the apex, then it will
    // start returning back to its normal state. toggle buttons are excluded
    // for obvious reasons
    if (state == BUTTON_STATE_APEXED && !(flags & BUTTON_FLAG_TOGGLE)) {
        wait += 1.0f / (pause * 60.0f);
        
        if (wait >= 1.0f) {
            wait = 0.0f;
            SwitchState(BUTTON_STATE_RELEASED);
        }
        
        UpdateParameters();
    }
    
    //if (state == BUTTON_STATE_APEXED) std::cout << "apexed " << std::endl;
    
    // special case for momentary toggle buttons
    // they need to wait a little bit at the apex, then they can be pushed again
    if (state == BUTTON_STATE_APEXED && flags & BUTTON_FLAG_TOGGLE && flags & BUTTON_FLAG_MOMENTARY) {
        wait += 1.0f / (pause * 60.0f);
        
        if (wait >= 1.0f) {
            wait = 0.0f;
            SwitchState(BUTTON_STATE_WAITING);
        }
    }
    
    if (state == BUTTON_STATE_RELEASED) {
        progress -= 1.0f / (speed * 60.0f);
        
        if (progress <= 0.0f) {
            progress = 0.0f;
            SwitchState(BUTTON_STATE_WAITING);
        }
        
        UpdateParameters();
    }
    
    //std::cout << "UPDATE " << GetTick() << " " << wait << " " << state << " " << speed << std::endl;
}

void Button::SwitchState(int state) {
switch (state) {
    case BUTTON_STATE_WAITING:
    
        // when a momentary toggle button returns to waiting state, it should
        // start spinning the opposide way. we do it by inverting its speed.
        // it's a hack.
        if (flags & BUTTON_FLAG_TOGGLE && flags & BUTTON_FLAG_MOMENTARY) {
            speed = -speed;
        }
    
        if (progress == 0.0f) FireSignal(Signal::END_CLOSE);
    
        RemoveUpdate();
        break;
    case BUTTON_STATE_PUSHED:
        AddUpdate();
        FireSignal(Signal::OPEN);
        break;
    case BUTTON_STATE_APEXED:
        wait = 0.0f;
        
        if (flags & BUTTON_FLAG_TOGGLE && !(flags & BUTTON_FLAG_MOMENTARY)) {
            RemoveUpdate();
        }
        
        if (progress == 1.0f) FireSignal(Signal::END_OPEN);
        if (progress == 0.0f) FireSignal(Signal::END_CLOSE);
        
        break;
    case BUTTON_STATE_RELEASED:
        AddUpdate();
        FireSignal(Signal::CLOSE);
        break;
    
    case BUTTON_STATE_REPUSHED:
        // this case will get called if and only
        // if the button is a momentary one, btw
        
        // this starts the rotation
        if (this->state == BUTTON_STATE_WAITING) {
            AddUpdate();
            //std::cout << "started rotato" << std::endl;
        }
        
        // this will have the button stuck at apex, if you keep pressing on it.
        // otherwise it would start the timer and return back down
        if (this->state == BUTTON_STATE_APEXED && (progress == 1.0f || progress == 0.0f) /*&& !(flags & BUTTON_FLAG_TOGGLE)*/) {
            wait = 0.0f;
            return;
        }
        
        break;
}
    this->state = state;
//std::cout << "switched state to " << state << std::endl;
}

void Button::MessageHandler(Message& msg){
    // button is pressed and it is not momentary
    if (msg.type == Message::ACTIVATE_ONCE && !(flags & BUTTON_FLAG_MOMENTARY) && !(flags & BUTTON_FLAG_LOCKED)) {
        
        // if button is in either fully pushed or waiting, toggle it
        if (state == BUTTON_STATE_APEXED) {
            SwitchState(BUTTON_STATE_RELEASED);
        } else if (state == BUTTON_STATE_WAITING) {
            SwitchState(BUTTON_STATE_PUSHED);
        }
    }
    
    // button is being pressed and it is momentary
    if (msg.type == Message::ACTIVATE && flags & BUTTON_FLAG_MOMENTARY && !(flags & BUTTON_FLAG_LOCKED)) {
        SwitchState(BUTTON_STATE_REPUSHED);
    }
    
    // opening and closing
    if (flags & BUTTON_FLAG_MOMENTARY) {
        if (msg.type == Message::OPEN) {
            progress += 1.0f / (speed * 60.0f);
            if (progress > 1.0f) progress = 1.0f;
            UpdateParameters();
        }
        
        if (msg.type == Message::CLOSE) {
            progress -= 1.0f / (speed * 60.0f);
            if (progress < 0.0f) progress = 0.0f;
            UpdateParameters();
        }
    } else {
        if ((msg.type == Message::OPEN || msg.type == Message::TOGGLE) && state == BUTTON_STATE_WAITING) {
            SwitchState(BUTTON_STATE_PUSHED);
        }
        
        if ((msg.type == Message::CLOSE || msg.type == Message::TOGGLE) && state == BUTTON_STATE_APEXED) {
            SwitchState(BUTTON_STATE_RELEASED);
        }
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