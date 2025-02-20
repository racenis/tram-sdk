// Tramway Drifting and Dungeon Exploration Simulator SDK Runtime

#include <components/player.h>

#include <config.h>

/**
 * @class tram::PlayerComponent components/player.h <components/player.h>
 * 
 * Translates keyboard/mouse inputs into inputs for ControllerComponent.
 * @see https://racenis.github.io/tram-sdk/documentation/components/player.html
 */

namespace tram {

template <> Pool<PlayerComponent> PoolProxy<PlayerComponent>::pool("PlayerComponent pool", COMPONENT_LIMIT_PLAYER);
template <> void Component<PlayerComponent>::init() { ptr = PoolProxy<PlayerComponent>::New(); }
template <> void Component<PlayerComponent>::yeet() { PoolProxy<PlayerComponent>::Delete(ptr); }

// technically incorrect, but it's not like we're going to have more than one
// player components running at the same time.. right?
static float pitch = 0.0f;
static float yaw = 0.0f;

PlayerComponent::PlayerComponent() {
    
}

void PlayerComponent::Init() {
    keydown.make(Event::KEYDOWN, this);
    keypress.make(Event::KEYPRESS, this);
    mouseposition.make(Event::CURSORPOS, this);
}

PlayerComponent::~PlayerComponent() {

}

void PlayerComponent::SetNoclip(bool value) {
    noclipping = value; 
    controller->SetCollisions(!noclipping);
}

void PlayerComponent::SetKeyboardLook(bool value) {
    keyboard_look = value; 
}

void PlayerComponent::UpdateLook() {
    pitch = pitch > 89.0f ? 89.0f : pitch < -89.0f ? -89.0f : pitch;
    
    this->look_rotation = quat(vec3(-glm::radians(pitch), -glm::radians(yaw), 0.0f));
    quat parent_rotation = quat(vec3(0.0f, -glm::radians(yaw), 0.0f));
    
    this->direction_facing = look_rotation * DIRECTION_FORWARD;
    
    Event::Post({
        .type = Event::LOOK_AT,
        .poster = parent->GetID(),
        .data = Event::AllocateData<Value>(look_rotation)
    });
    
    controller->SetLookDirection(look_rotation);
    parent->UpdateTransform(parent->GetLocation(), parent_rotation);
}



void PlayerComponent::EventHandler(Event &event) {
    using namespace tram::UI;
    
    // Map cursor position into camera and entity orientation.
    if (event.type == Event::CURSORPOS && !keyboard_look) {
        yaw += PollKeyboardAxisDelta(KEY_MOUSE_X) * CAMERA_MULTIPLIER;
        pitch += PollKeyboardAxisDelta(KEY_MOUSE_Y) * CAMERA_MULTIPLIER;
        pitch = pitch > 89.0f ? 89.0f : pitch < -89.0f ? -89.0f : pitch;
        this->look_rotation = quat(vec3(-glm::radians(pitch), -glm::radians(yaw), 0.0f));
        quat parent_rotation = quat(vec3(0.0f, -glm::radians(yaw), 0.0f));
        
        this->direction_facing = look_rotation * DIRECTION_FORWARD;
        
        Event::Post({
            .type = Event::LOOK_AT,
            .poster = parent->GetID(),
            .data = Event::AllocateData<Value>(look_rotation)
        });
        
        controller->SetLookDirection(look_rotation);
        parent->UpdateTransform(parent->GetLocation(), parent_rotation);
    }

    if (event.type == Event::KEYPRESS && keyboard_look) {
        switch (event.subtype) {
            case UI::KEY_ACTION_LEFT:
                yaw -= 100.0f * GetDeltaTime(); break;
            case UI::KEY_ACTION_RIGHT:
                yaw += 100.0f * GetDeltaTime(); break;
            case UI::KEY_ACTION_UP:
                pitch -= 100.0f * GetDeltaTime(); break;
            case UI::KEY_ACTION_DOWN:
                pitch += 100.0f * GetDeltaTime(); break;
                
        }
        
        UpdateLook();
    }

    if (event.type == Event::KEYDOWN && event.subtype == KEY_ACTION_JUMP) {
        controller->Jump();
    }
    
    if (event.type == Event::KEYPRESS) {
        if (noclipping) {
            if (event.subtype == KEY_ACTION_FORWARD)        controller->Move(glm::inverse(parent->GetRotation()) * direction_facing);
            if (event.subtype == KEY_ACTION_BACKWARD)       controller->Move(glm::inverse(parent->GetRotation()) * -direction_facing);
            if (event.subtype == KEY_ACTION_STRAFE_LEFT)    controller->Move(-DIRECTION_SIDE);
            if (event.subtype == KEY_ACTION_STRAFE_RIGHT)   controller->Move(DIRECTION_SIDE);
        } else {
            if (event.subtype == KEY_ACTION_FORWARD)        controller->Move(DIRECTION_FORWARD);
            if (event.subtype == KEY_ACTION_BACKWARD)       controller->Move(-DIRECTION_FORWARD);
            if (event.subtype == KEY_ACTION_STRAFE_LEFT)    controller->Move(-DIRECTION_SIDE);
            if (event.subtype == KEY_ACTION_STRAFE_RIGHT)   controller->Move(DIRECTION_SIDE);
        }
        
        
        if (event.subtype == KEY_ACTION_CROUCH)         controller->Crouch();
        if (event.subtype == KEY_ACTION_SPRINT)         controller->Run();
        if (event.subtype == KEY_ACTION_FLY)            controller->Fly();
    }
}

}