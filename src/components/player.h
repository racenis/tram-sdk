// TRAMWAY DRIFT AND DUNGEON EXPLORATION SIMULATOR 2022
// All rights reserved.

#ifndef TRAM_SDK_COMPONENTS_PLAYERCOMPONENT_H
#define TRAM_SDK_COMPONENTS_PLAYERCOMPONENT_H

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
    void SetParent(Entity* ent) { parent = ent; }
    void SetControllerComponent (ControllerComponent* comp) {controller = comp; }
    void IgnoreMove() { is_move = false; }
    void StopIgnoreMove() { is_move = true; }
    void Start() {}
    void MoveUpdate() { assert(cell_loader); cell_loader->UpdateLocation(parent->GetLocation()); }
private:
    Entity* parent = nullptr; // whatuses?
    
    EventListener keydown;
    EventListener keyup;
    EventListener mouseposition;
    
    ControllerComponent* controller = nullptr;
    Loader* cell_loader = nullptr;

    bool move_forward = false;
    bool move_backward = false;
    bool move_left = false;
    bool move_right = false;
    
    bool is_crouch = false;
    bool is_run = false;

    bool is_move = true;
};

}

#endif // TRAM_SDK_COMPONENTS_PLAYERCOMPONENT_H