// TRAMWAY DRIFT AND DUNGEON EXPLORATION SIMULATOR 2022
// All rights reserved.

#include <framework/core.h>
#include <entities/lamp.h>
#include <components/light.h>

#include <framework/serialization.h>

namespace tram {

enum {
    FIELD_COLOR_R,
    FIELD_COLOR_G,
    FIELD_COLOR_B,
    FIELD_DISTANCE
};

static const uint32_t fields[4] = {
    TYPE_FLOAT32,
    TYPE_FLOAT32,
    TYPE_FLOAT32,
    TYPE_FLOAT32
}; 

void Lamp::Register() {
    Entity::RegisterType(
        "lamp", 
        [](const SharedEntityData& a, const SerializedFieldArray& b) -> Entity* { return new Lamp(a, b); },
        [](Entity* a) { delete a; },
        fields,
        4
    );
}

Lamp::Lamp(const SharedEntityData& shared_data, const SerializedFieldArray& field_array) : Entity(shared_data) {
    color_r = field_array[FIELD_COLOR_R];
    color_g = field_array[FIELD_COLOR_G];
    color_b = field_array[FIELD_COLOR_B];
    distance = field_array[FIELD_DISTANCE];
}


void Lamp::UpdateParameters () {
    if (!is_loaded) return;
    light->SetLocation(location);
}

void Lamp::SetParameters () {
    UpdateParameters();
}

void Lamp::Load () {
    light.make();
    light->SetColor({color_r, color_g, color_b});
    light->SetDistance(distance);

    light->Init();
    is_loaded = true;

    UpdateParameters();
}

void Lamp::Unload () {
    is_loaded = false;

    Serialize();

    light.clear();
}

void Lamp::Serialize () {
    glm::vec3 light_color;
    float light_distance;
    
    light_color = light->GetColor(); // idk if this actually works
    light_distance = light->GetDistance();
    
    color_r = light_color.r;
    color_g = light_color.g;
    color_b = light_color.b;
    distance = light_distance;
}

void Lamp::MessageHandler (Message& msg) {
    return;
}

}