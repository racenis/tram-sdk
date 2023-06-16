#include <components/playercomponent.h>

namespace tram {

template <> Pool<PlayerComponent> PoolProxy<PlayerComponent>::pool ("player component pool", 5, false);

PlayerComponent::PlayerComponent() {}

void PlayerComponent::Init() {
    keydown.make(Event::KEYDOWN, this);
    keyup.make(Event::CURSORPOS, this);
    keypress.make(Event::KEYUP, this);
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

    if (event.type == Event::CURSORPOS) {
        //controller->SetDirection(glm::quat(glm::vec3(0.0f, -glm::radians(CAMERA_YAW), 0.0f)));
        //auto p_pos = parent->GetLocation();
        //auto p_rot = glm::quat(glm::vec3(0.0f, -glm::radians(CAMERA_YAW), 0.0f));
        //parent->UpdateTransform(p_pos, p_rot);
        
        // this is dumb
        // camera system shouldn't poll mouse position directly and then
        // update the render system. instead the player component should
        // poll the input system and then update the camera.
        // or something, idk.
        // TODO: fix
        
        look_direction = Render::GetCameraRotation() * DIRECTION_FORWARD;
        look_position = Render::GetCameraPosition();
        
        if (holding) {
            vec3* data = (vec3*) Message::AllocateData(sizeof(vec3));
            *data = look_position + 2.0f * look_direction;
            
            Message::Send({
                Message::MOVE_PICK_UP,
                holding->GetID(),
                parent->GetID(),
                data
            });
        }
        
        return;
    }
    
    if (event.type == Event::KEYDOWN && event.subtype == KEY_ACTION_ACTIVATE) {
        if (holding) {
            std::cout << "dropped" << std::endl;
            
            Message::Send({
                Message::MOVE_PICK_UP,
                holding->GetID(),
                parent->GetID(),
                nullptr
            });
            
            holding = nullptr;
        } else {
            auto res = Physics::Raycast(look_position, look_position + (2.0f * look_direction), -1 ^ Physics::COLL_PLAYER);
        
            if (res.collider) {
                std::cout << "picked up" << std::endl;
                
                holding = res.collider->GetParent();
            } else {
                std::cout << "missed" << std::endl;
            }
        }
    }

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