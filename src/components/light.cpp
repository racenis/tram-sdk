#include <components/light.h>

#include <render/aabb.h>
#include <render/api.h>

namespace tram {

template <> Pool<LightComponent> PoolProxy<LightComponent>::pool ("light component pool", 250, false);   

void LightComponent::Init () {
    light = Render::API::MakeLight();
    
    Render::LightTree::AddLight(light, location, distance);
    
    is_init = true;
    is_ready = true;
    
    Update();    
}

LightComponent::~LightComponent () {
    Render::API::DeleteLight(light);
    Render::LightTree::RemoveLight(light);
    
    light.generic = nullptr;
    is_ready = false;
}

void LightComponent::Update () {
    if (is_ready) {
        Render::API::SetLightParameters(light, location, color, distance, direction, exponent);
        Render::LightTree::RemoveLight(light);
        Render::LightTree::AddLight(light, location, distance);
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
    this->distance = dist;
    Update();
}

void LightComponent::SetDirection(vec3 direction) {
    this->direction = glm::normalize(direction);
    Update();
}

void LightComponent::SetExponent(float exponent) {
    this->exponent = exponent;
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
