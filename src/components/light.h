// TRAMWAY DRIFT AND DUNGEON EXPLORATION SIMULATOR 2022
// All rights reserved.

#ifndef TRAM_SDK_COMPONENTS_LIGHTCOMPONENT_H
#define TRAM_SDK_COMPONENTS_LIGHTCOMPONENT_H

#include <framework/entitycomponent.h>
#include <render/render.h>

namespace tram {

class LightComponent : public EntityComponent {
public:
    void Init ();
    void Start () {}
    
    void SetLocation (vec3 location);
    void SetColor (Render::color_t color);
    void SetDistance (float dist);
    
    vec3 GetColor ();
    float GetDistance ();

    void EventHandler(Event &event) { return; }    
    ~LightComponent();
protected:
    void Update();
    
    Render::light_t light = {.generic = nullptr};
    vec3 location;
    Render::color_t color;
    float distance;
};

}

#endif // TRAM_SDK_COMPONENTS_LIGHTCOMPONENT_H