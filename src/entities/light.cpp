// TRAMWAY DRIFT AND DUNGEON EXPLORATION SIMULATOR 2022
// All rights reserved.

#include <framework/core.h>
#include <entities/light.h>
#include <components/light.h>

#include <framework/serialization.h>

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

Light::Light(const SharedEntityData& shared_data, const ValueArray& field_array) : Entity(shared_data) {
    color = field_array[FIELD_COLOR];
    distance = field_array[FIELD_DISTANCE];
    direction = field_array[FIELD_DIRECTION];
    exponent = field_array[FIELD_EXPONENT];    
}

void Light::UpdateParameters () {
    if (!is_loaded) return;
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
    is_loaded = true;

    UpdateParameters();
}

void Light::Unload () {
    is_loaded = false;

    Serialize();

    light.clear();
}

void Light::Serialize () {
    glm::vec3 light_color;
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