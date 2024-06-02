// TRAMWAY DRIFT AND DUNGEON EXPLORATION SIMULATOR 2022
// All rights reserved.

#include <framework/core.h>
#include <entities/light.h>
#include <components/light.h>

#include <framework/serialization.h>

namespace tram {

enum {
    FIELD_COLOR_R,
    FIELD_COLOR_G,
    FIELD_COLOR_B,
    FIELD_DISTANCE,
    FIELD_DIRECTION_X,
    FIELD_DIRECTION_Y,
    FIELD_DIRECTION_Z,
    FIELD_EXPONENT
};

static const uint32_t fields[8] = {
    TYPE_FLOAT32,
    TYPE_FLOAT32,
    TYPE_FLOAT32,
    TYPE_FLOAT32,
    TYPE_FLOAT32,
    TYPE_FLOAT32,
    TYPE_FLOAT32,
    TYPE_FLOAT32
}; 

void Light::Register() {
    Entity::RegisterType(
        "light", 
        [](const SharedEntityData& a, const ValueArray& b) -> Entity* { return new Light(a, b); },
        [](Entity* a) { delete a; },
        fields,
        8
    );
}

Light::Light(const SharedEntityData& shared_data, const ValueArray& field_array) : Entity(shared_data) {
    color_r = field_array[FIELD_COLOR_R];
    color_g = field_array[FIELD_COLOR_G];
    color_b = field_array[FIELD_COLOR_B];
    distance = field_array[FIELD_DISTANCE];
    direction_x = field_array[FIELD_DIRECTION_X];
    direction_y = field_array[FIELD_DIRECTION_Y];
    direction_z = field_array[FIELD_DIRECTION_Z];
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
    light->SetColor({color_r, color_g, color_b});
    light->SetDistance(distance);
    light->SetDirection({direction_x, direction_y, direction_z});
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
    
    color_r = light_color.r;
    color_g = light_color.g;
    color_b = light_color.b;
    distance = light_distance;
}

void Light::MessageHandler (Message& msg) {
    return;
}

}