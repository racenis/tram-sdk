// TRAMWAY DRIFT AND DUNGEON EXPLORATION SIMULATOR 2022
// All rights reserved.

#ifndef TRAM_SDK_COMPONENTS_PLAYERCOMPONENT_H
#define TRAM_SDK_COMPONENTS_PLAYERCOMPONENT_H

#include <framework/ui.h>
#include <framework/event.h>
#include <framework/entity.h>

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
    void SetControllerComponent (ControllerComponent* comp) { controller = comp; }
    void Start() {}
    void SetNoclip(bool value);
private:
    EventListener keydown;
    EventListener keypress;
    EventListener mouseposition;
    
    vec3 direction_facing = {0.0f, 0.0f, 0.0f};
    
    bool noclipping = false;
    
    ControllerComponent* controller = nullptr;
};

}

#endif // TRAM_SDK_COMPONENTS_PLAYERCOMPONENT_H