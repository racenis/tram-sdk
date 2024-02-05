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
    void Start() {}
    void MoveUpdate() { assert(cell_loader); cell_loader->UpdateLocation(parent->GetLocation()); }
private:
    Entity* parent = nullptr; // whatuses?
    
    EventListener keydown;
    EventListener keypress;
    EventListener mouseposition;
    
    ControllerComponent* controller = nullptr;
    Loader* cell_loader = nullptr;
};

}

#endif // TRAM_SDK_COMPONENTS_PLAYERCOMPONENT_H