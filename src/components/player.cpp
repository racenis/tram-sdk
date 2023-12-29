#include <components/player.h>

namespace tram {

template <> Pool<PlayerComponent> PoolProxy<PlayerComponent>::pool ("player component pool", 5, false);

// technically incorrect, but it's not like we're going to have more than one
// player components running at the same time.. right?
static float cursorchangex = 0.0f;
static float cursorchangey = 0.0f;
static float cursorx_last = 0.0f;
static float cursory_last = 0.0f;
static float pitch = 0.0f;
static float yaw = 0.0f;

PlayerComponent::PlayerComponent() {
    cursorx_last = PollKeyboardAxis(UI::KEY_MOUSE_X);
    cursory_last = PollKeyboardAxis(UI::KEY_MOUSE_Y);
}

void PlayerComponent::Init() {
    keydown.make(Event::KEYDOWN, this);
    keyup.make(Event::KEYUP, this);
    keypress.make(Event::KEYPRESS, this);
    mouseposition.make(Event::CURSORPOS, this);

    cell_loader = PoolProxy<Loader>::New();
    cell_loader->SetLocation(parent->GetLocation());
}

PlayerComponent::~PlayerComponent() {
    PoolProxy<Loader>::Delete(cell_loader);
    cell_loader = nullptr;
}

void PlayerComponent::EventHandler (Event &event) {
    using namespace tram::UI;
    using enum tram::ControllerComponent::Action;
    using enum tram::ControllerComponent::ActionModifier;

    // Map cursor position into camera and entity orientation.
    if (event.type == Event::CURSORPOS) {
        cursorchangex = PollKeyboardAxis(UI::KEY_MOUSE_X) - cursorx_last;
        cursorchangey = PollKeyboardAxis(UI::KEY_MOUSE_Y) - cursory_last;
        cursorx_last = PollKeyboardAxis(UI::KEY_MOUSE_X);
        cursory_last = PollKeyboardAxis(UI::KEY_MOUSE_Y);

        yaw += cursorchangex * UI::CAMERA_SENSITIVITY;
        pitch += cursorchangey * UI::CAMERA_SENSITIVITY;
        pitch = pitch > 89.0f ? 89.0f : pitch < -89.0f ? -89.0f : pitch;
        quat look_rotation = quat(vec3(-glm::radians(pitch), -glm::radians(yaw), 0.0f));
        quat parent_rotation = quat(vec3(0.0f, -glm::radians(yaw), 0.0f));
        
        controller->SetLookDirection(look_rotation);
        parent->UpdateTransform(parent->GetLocation(), parent_rotation);
        
        return;
    }

    // Compute move direction.
    if (is_move && (event.type == Event::KEYDOWN || event.type == Event::KEYUP)) {
        bool move_value = event.type == Event::KEYDOWN;
        
        if (event.subtype == KEY_ACTION_FORWARD)    move_forward =  move_value;
        if (event.subtype == KEY_ACTION_BACKWARD)   move_backward = move_value;
        if (event.subtype == KEY_ACTION_STRAFE_LEFT)       move_left =     move_value;
        if (event.subtype == KEY_ACTION_STRAFE_RIGHT)      move_right =    move_value;
        
        if (event.subtype == KEY_ACTION_CROUCH) is_crouch =     move_value;
        if (event.subtype == KEY_ACTION_SPRINT) is_run =        move_value;
    }

    // I have literally no idea what is going on in here

    auto action = ACTION_IDLE;
    auto modifier = ACTIONMODIFIER_NONE;

    if (move_forward) {
        action = ACTION_WALK;
        modifier = ACTIONMODIFIER_FORWARD;
    }
    
    if (move_backward) {
        action = ACTION_WALK;
        modifier = ACTIONMODIFIER_BACKWARD;
    }
    
    if (move_left) {
        action = ACTION_WALK;
        modifier = ACTIONMODIFIER_LEFT;
    }
    
    if (move_right) {
        action = ACTION_WALK;
        modifier = ACTIONMODIFIER_RIGHT;
    }
    
    if (move_forward && move_left) {
        action = ACTION_WALK;
        modifier = ACTIONMODIFIER_FORWARD_LEFT;
    }
    
    if (move_forward && move_right) {
        action = ACTION_WALK;
        modifier = ACTIONMODIFIER_FORWARD_RIGHT;
    }
    
    if (move_backward && move_left) {
        action = ACTION_WALK;
        modifier = ACTIONMODIFIER_BACKWARD_LEFT;
    }
    
    if (move_backward && move_right) {
        action = ACTION_WALK;
        modifier = ACTIONMODIFIER_BACKWARD_RIGHT;
    }

    if (event.subtype == KEY_ACTION_JUMP && event.type == Event::KEYDOWN) {
        action = ACTION_JUMP;
        modifier = ACTIONMODIFIER_NONE;
    }
    
    if (is_run && action == ACTION_WALK) action = ACTION_RUN;
    if (is_crouch && action == ACTION_WALK) action = ACTION_CROUCH;
    if (is_crouch && action == ACTION_IDLE) action = ACTION_CROUCH_IDLE;
    
    controller->Act(action, modifier);
}

}