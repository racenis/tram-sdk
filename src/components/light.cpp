// Tramway Drifting and Dungeon Exploration Simulator SDK Runtime

#include <components/light.h>

#include <render/scene.h>
#include <render/api.h>

#include <config.h>

/**
 * @class tram::LightComponent
 * 
 * Provides a real-time illumination for the scene.
 * @see https://racenis.github.io/tram-sdk/documentation/components/light.html
 */

/* 
 * TODO: Same as with AudioComponents -- don't like how the 3D models are set up.
 */

namespace tram {

template <> Pool<LightComponent> PoolProxy<LightComponent>::pool("LightComponent pool", COMPONENT_LIMIT_LIGHT, false);   

void LightComponent::Init() {
    light = Render::API::MakeLight();
    
    Render::LightTree::AddLight(light, location, distance);
    
    SetupModel();
    
    is_init = true;
    is_ready = true;
    
    Update();    
}

LightComponent::~LightComponent() {
    Render::API::DeleteLight(light);
    Render::LightTree::RemoveLight(light);
    
    light = {};
    is_ready = false;
}

void LightComponent::Update() {
    if (is_ready) {
        Render::API::SetLightParameters(light, location, color, distance, direction, exponent);
        Render::LightTree::RemoveLight(light);
        Render::LightTree::AddLight(light, location, distance);
        
        if (model) {
            model->SetLocation(location);
            model->SetColor(color);
        }
    }
}

/// Sets the location of the light.
void LightComponent::SetLocation(vec3 location) {
    this->location = location;
    Update();
}

/// Sets the color of the light.
void LightComponent::SetColor(Render::color_t color) {
    this->color = color;
    Update();
}

/// Sets the distance of the light.
/// Sets how far the light will travel.
void LightComponent::SetDistance(float dist) {
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
Render::color_t LightComponent::GetColor() {
    return this->color;
}

/// Returns the distance of the light.
float LightComponent::GetDistance() {
    return this->distance;
}

static bool draw_light = false;

void LightComponent::SetupModel() {
    if (draw_light) {
        model.make();
        model->SetModel("dev/light");
        model->SetLocation(this->location);
        model->SetParent(this->parent);
        model->SetColor(this->color);
        model->SetLightmap("fullbright");
        model->SetRenderDebug(false);
        model->Init();
    } else {
        model.clear();
    }
}

/// Checks whether the lights are drawn.
/// Check SetLightDraw() for more info.
bool LightComponent::IsLightDraw() {
    return draw_light;
}

/// Sets the drawing oflights.
/// If set to true, each light component will initialize a 3D model, which will
/// allow you to see the posiiton of the light sources. Useful for debugging.
void LightComponent::SetLightDraw(bool draw) {
    if (draw_light == draw) return;
    draw_light = draw;
    
    for (auto& source : PoolProxy<LightComponent>::GetPool()) {
        source.SetupModel();
    }
}

}
