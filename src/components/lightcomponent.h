// TRAMWAY DRIFT AND DUNGEON EXPLORATION SIMULATOR 2022
// All rights reserved.

#ifndef COMPONENTS_LIGHTCOMPONENT_H
#define COMPONENTS_LIGHTCOMPONENT_H

#include <render/api.h>

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
    
    Render::light_t light = nullptr;
    vec3 location;
    Render::color_t color;
    float distance;
};

}

#endif // COMPONENTS_LIGHTCOMPONENT_H