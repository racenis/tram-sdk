#include <components/player.h>

namespace tram {

template <> Pool<PlayerComponent> PoolProxy<PlayerComponent>::pool ("player component pool", 5, false);

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

    cell_loader = PoolProxy<Loader>::New();
    cell_loader->SetLocation(parent->GetLocation());
}

PlayerComponent::~PlayerComponent() {
    PoolProxy<Loader>::Delete(cell_loader);
    cell_loader = nullptr;
}

void PlayerComponent::EventHandler(Event &event) {
    using namespace tram::UI;
    //using enum tram::ControllerComponent::Action;
    //using enum tram::ControllerComponent::ActionModifier;

    // Map cursor position into camera and entity orientation.
    if (event.type == Event::CURSORPOS) {
        yaw += PollKeyboardAxisDelta(KEY_MOUSE_X) * UI::CAMERA_SENSITIVITY * GetDeltaTime();
        pitch += PollKeyboardAxisDelta(KEY_MOUSE_Y) * UI::CAMERA_SENSITIVITY * GetDeltaTime();
        pitch = pitch > 89.0f ? 89.0f : pitch < -89.0f ? -89.0f : pitch;
        quat look_rotation = quat(vec3(-glm::radians(pitch), -glm::radians(yaw), 0.0f));
        quat parent_rotation = quat(vec3(0.0f, -glm::radians(yaw), 0.0f));
        
        //std::cout << yaw <<" " << pitch << std::endl;
        //std::cout << PollKeyboardAxisDelta(KEY_MOUSE_X) <<" delta " << PollKeyboardAxisDelta(KEY_MOUSE_Y) << std::endl;
        
        controller->SetLookDirection(look_rotation);
        parent->UpdateTransform(parent->GetLocation(), parent_rotation);
    }

    if (event.type == Event::KEYDOWN && event.subtype == KEY_ACTION_JUMP) {
        controller->Jump();
    }
    
    if (event.type == Event::KEYPRESS) {
        if (event.subtype == KEY_ACTION_FORWARD)        controller->Move(DIRECTION_FORWARD);
        if (event.subtype == KEY_ACTION_BACKWARD)       controller->Move(-DIRECTION_FORWARD);
        if (event.subtype == KEY_ACTION_STRAFE_LEFT)    controller->Move(-DIRECTION_SIDE);
        if (event.subtype == KEY_ACTION_STRAFE_RIGHT)   controller->Move(DIRECTION_SIDE);
        
        if (event.subtype == KEY_ACTION_CROUCH)         controller->Crouch();
        if (event.subtype == KEY_ACTION_SPRINT)         controller->Run();
    }
}

}