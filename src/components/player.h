// Tramway Drifting and Dungeon Exploration Simulator SDK Runtime

#ifndef TRAM_SDK_COMPONENTS_PLAYERCOMPONENT_H
#define TRAM_SDK_COMPONENTS_PLAYERCOMPONENT_H

#include <framework/ui.h>
#include <framework/event.h>
#include <framework/entity.h>

#include <components/controller.h>

#include <framework/logging.h>

// this is for holding
// re: for holding what??
// TODO: find out
#include <render/render.h>
#include <framework/message.h>

namespace tram {

class PlayerComponent : public EntityComponent {
public:
    static PlayerComponent* Make();
    static void Yeet(PlayerComponent* component);
    
    void SetControllerComponent (ControllerComponent* comp) { controller = comp; }
    void Start();
    void SetNoclip(bool value);
    void SetKeyboardLook(bool value);
    
    void EventHandler(Event &event);
    
    inline quat GetLookRotation() { return look_rotation; }
    inline vec3 GetDirectionFacing() { return direction_facing; }
private:
    EventListener keydown;
    EventListener keypress;
    EventListener mouseposition;
    
    quat look_rotation = {1.0f, 0.0f, 0.0f, 0.0f};
    vec3 direction_facing = {0.0f, 0.0f, -1.0f};
    
    bool noclipping = false;
    bool keyboard_look = false;
    
    void UpdateLook();
    
    PlayerComponent();
    ~PlayerComponent();
    
    ControllerComponent* controller = nullptr;
};

}

#endif // TRAM_SDK_COMPONENTS_PLAYERCOMPONENT_H