#include <components/light.h>

#include <render/api.h>

namespace tram {

template <> Pool<LightComponent> PoolProxy<LightComponent>::pool ("light component pool", 250, false);   

void LightComponent::Init () {
    light = Render::API::MakeLight();
    
    is_init = true;
    is_ready = true;
    
    Update();    
}

LightComponent::~LightComponent () {
    Render::API::DeleteLight(light);
    
    light = nullptr;
    is_ready = false;
}

void LightComponent::Update () {
    if (is_ready) {
        Render::API::SetLightParameters(light, location, color, distance);
    }
}

/// Sets the location of the light.
void LightComponent::SetLocation (vec3 location) {
    this->location = location;
    Update();
}

/// Sets the color of the light.
void LightComponent::SetColor (Render::color_t color) {
    this->color = color;
    Update();
}

/// Sets the distance of the light.
/// Sets how far the light will travel.
void LightComponent::SetDistance (float dist) {
    distance = dist;
    Update();
}

/// Returns the color of the light.
Render::color_t LightComponent::GetColor () {
    return this->color;
}

/// Returns the distance of the light.
float LightComponent::GetDistance () {
    return this->distance;
}

}
