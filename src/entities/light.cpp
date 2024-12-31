// Tramway Drifting and Dungeon Exploration Simulator SDK Runtime

#include <framework/core.h>
#include <entities/light.h>
#include <components/light.h>

#include <framework/serialization.h>

/**
 * @class tram::Light entities/light.h <entities/light.h>
 * 
 * LightComponent wrapper.
 * @see https://racenis.github.io/tram-sdk/documentation/entities/light.html
 */

namespace tram {

enum {
    FIELD_COLOR,
    FIELD_DISTANCE,
    FIELD_DIRECTION,
    FIELD_EXPONENT
};

void Light::Register() {
    Entity::RegisterType(
        "light", 
        [](const SharedEntityData& a, const ValueArray& b) -> Entity* { return new Light(a, b); },
        [](Entity* a) { delete a; },
        {
            {FIELD_COLOR,       TYPE_VEC3,      FIELD_SERIALIZE},
            {FIELD_DISTANCE,    TYPE_FLOAT32,   FIELD_SERIALIZE},
            {FIELD_DIRECTION,   TYPE_VEC3,      FIELD_SERIALIZE},
            {FIELD_EXPONENT,    TYPE_FLOAT32,   FIELD_SERIALIZE}
        }
    );
}

name_t Light::GetType() {
    return "light";
}

Light::Light(const SharedEntityData& shared_data, const ValueArray& field_array) : Entity(shared_data) {
    color = field_array[FIELD_COLOR];
    distance = field_array[FIELD_DISTANCE];
    direction = field_array[FIELD_DIRECTION];
    exponent = field_array[FIELD_EXPONENT];    
}

void Light::UpdateParameters () {
    if (!IsLoaded()) return;
    light->SetLocation(location);
}

void Light::SetParameters () {
    UpdateParameters();
}

void Light::Load () {
    light.make();
    light->SetColor(color);
    light->SetDistance(distance);
    light->SetDirection(direction);
    light->SetExponent(exponent);

    light->Init();
    flags |= LOADED;

    UpdateParameters();
}

void Light::Unload () {
    flags &= ~LOADED;

    Serialize();

    light.clear();
}

void Light::Serialize () {
    vec3 light_color;
    float light_distance;
    
    light_color = light->GetColor(); // idk if this actually works
    light_distance = light->GetDistance();
    
    color = light_color;
    distance = light_distance;
}

void Light::MessageHandler (Message& msg) {
    return;
}

}