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
    
    inline quat GetLookRotation() { return look_rotation; }
    inline vec3 GetDirectionFacing() { return direction_facing; }
private:
    EventListener keydown;
    EventListener keypress;
    EventListener mouseposition;
    
    quat look_rotation = {1.0f, 0.0f, 0.0f, 0.0f};
    vec3 direction_facing = {0.0f, 0.0f, -1.0f};
    
    bool noclipping = false;
    
    ControllerComponent* controller = nullptr;
};

}

#endif // TRAM_SDK_COMPONENTS_PLAYERCOMPONENT_H