// Tramway Drifting and Dungeon Exploration Simulator SDK Runtime

#include <components/light.h>

#include <render/scene.h>
#include <render/api.h>

#include <framework/settings.h>
#include <framework/event.h>

#include <config.h>

/**
 * @class tram::LightComponent components/light.h <components/light.h>
 * 
 * Provides a real-time illumination for the scene.
 * @see https://racenis.github.io/tram-sdk/documentation/components/light.html
 */

/* 
 * TODO: Same as with AudioComponents -- don't like how the 3D models are set up.
 */

namespace tram {

template <> Pool<LightComponent> PoolProxy<LightComponent>::pool("LightComponent pool", COMPONENT_LIMIT_LIGHT);   
template <> void Component<LightComponent>::init() { ptr = LightComponent::Make(); }
template <> void Component<LightComponent>::yeet() { LightComponent::Yeet(ptr); }

static Settings::Property<bool> draw_light = {false, "light-draw-icon", Settings::NONE};

static EventListener frame_event;

static void check_event(const char*) {
    if (draw_light) {
        frame_event.make(Event::FRAME, [](Event&) {
            for (auto& light : PoolProxy<LightComponent>::GetPool()) {
                if (glm::distance(light.GetLocation(), Render::GetViewPosition()) < 15.0f) {
                    Render::AddIcon(light.GetLocation(), 8);
                }
            }
        });
    } else {
        frame_event.clear();
    } 
}

void LightComponent::Start() {
    light = Render::API::MakeLight();
    
    Render::LightTree::AddLight(light, location, distance);

    is_init = true;
    is_ready = true;
    
    Update();
}

LightComponent::~LightComponent() {
    Render::API::YeetLight(light);
    Render::LightTree::RemoveLight(light);
    
    light = {};
    is_ready = false;
}

void LightComponent::Update() {
    if (is_ready) {
        Render::API::SetLightParameters(light, location, color, distance, direction, exponent);
        Render::LightTree::RemoveLight(light);
        Render::LightTree::AddLight(light, location, distance);
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

/// Creates a new LightComponent.
LightComponent* LightComponent::Make() {
    static bool made = false;
    if (!made) {
        Settings::SetCallback("light-draw-icon", check_event);
        check_event(nullptr);
        
        made = true;
    }
    
    LightComponent* ptr = PoolProxy<LightComponent>::GetPool().allocate();
    new(ptr) LightComponent();
    return ptr;
}

/// Deletes an LightComponent.
void LightComponent::Yeet(LightComponent* component) {
    component->~LightComponent();
    PoolProxy<LightComponent>::GetPool().deallocate(component);
}

}
