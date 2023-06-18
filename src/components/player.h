// TRAMWAY DRIFT AND DUNGEON EXPLORATION SIMULATOR 2022
// All rights reserved.

#ifndef COMPONENTS_PLAYERCOMPONENT_H
#define COMPONENTS_PLAYERCOMPONENT_H

#include <framework/ui.h>
#include <framework/event.h>
#include <framework/entity.h>
#include <framework/loader.h>

#include <components/controller.h>

#include <framework/logging.h>

// this is for holding
#include <render/render.h>
#include <framework/message.h>

namespace tram {

class PlayerComponent : public EntityComponent {
public:
    void EventHandler (Event &event);
    void Init();
    PlayerComponent();
    ~PlayerComponent();
    void SetParent(Entity* ent){
        parent = ent;
    }
    void SetControllerComponent (ControllerComponent* comp){controller = comp;};
    void SetVehicle (uint64_t vehicleID){vehicle = vehicleID;};
    void IgnoreMove(){
        is_move = false;
    }
    void StopIgnoreMove(){
        is_move = true;
    }
    void Start(){}
    void MoveUpdate() { assert(cell_loader); cell_loader->UpdateLocation(parent->GetLocation()); }
private:
    Entity* parent = nullptr; // whatuses?
    
    EventListener keydown;
    EventListener keyup;
    EventListener keypress;
    EventListener mouseposition;
    
    //listener_t keydown = 0;
    //listener_t keyup = 0;
    //listener_t listener = 0;
    //listener_t mouselistener = 0;
    ControllerComponent* controller = nullptr;
    Loader* cell_loader = nullptr;

    uint64_t vehicle = 0;

    // wtf are these
    const glm::vec3 forwarddir = glm::vec3(0.0f, 0.0f, -1.0f);
    const glm::vec3 sidedir = glm::vec3(1.0f, 0.0f, 0.0f);
    const glm::vec3 updir = glm::vec3(0.0f, 1.0f, 0.0f);
    glm::quat direction;
    
    vec3 look_direction = {0.0f, 1.0f, 0.0f};
    vec3 look_position = {0.0f, 0.0f, 0.0f};
    
    bool move_forward = false;
    bool move_backward = false;
    bool move_left = false;
    bool move_right = false;
    
    bool is_crouch = false;
    bool is_run = false;

    bool is_move = true;
};

}

#endif // COMPONENTS_PLAYERCOMPONENT_H