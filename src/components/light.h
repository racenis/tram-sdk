// TRAMWAY DRIFT AND DUNGEON EXPLORATION SIMULATOR 2022
// All rights reserved.

#ifndef TRAM_SDK_COMPONENTS_LIGHTCOMPONENT_H
#define TRAM_SDK_COMPONENTS_LIGHTCOMPONENT_H

#include <framework/entitycomponent.h>
#include <components/render.h>
#include <render/render.h>

namespace tram {

class LightComponent : public EntityComponent {
public:
    void Init ();
    void Start () {}
    
    void SetLocation(vec3 location);
    void SetColor(Render::color_t color);
    void SetDistance(float dist);
    void SetDirection(vec3 direction);
    void SetExponent(float exponent);
    
    vec3 GetColor();
    float GetDistance();

    void EventHandler(Event &event) { return; }
    ~LightComponent();
    
    static bool IsLightDraw();
    static void SetLightDraw(bool);
protected:
    void Update();
    
    Render::light_t light = {.generic = nullptr};
    vec3 location = {0.0f, 0.0f, 0.0f};
    Render::color_t color = {1.0f, 1.0f, 1.0f};
    float distance = 15.0f;
    vec3 direction = {0.0f, 0.0f, -1.0f};
    float exponent = 0.0f;
    
    void SetupModel();
    Component<RenderComponent> model;
};

}

#endif // TRAM_SDK_COMPONENTS_LIGHTCOMPONENT_H