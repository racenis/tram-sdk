// TRAMWAY DRIFT AND DUNGEON EXPLORATION SIMULATOR 2022
// All rights reserved.

#include <framework/core.h>
#include <entities/lamp.h>
#include <components/lightcomponent.h>

namespace tram {

Lamp::Lamp (std::string_view& str) : Entity (str) {
    data.make();
    data->FromString(str);
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
    light->SetColor({data->color_r, data->color_g, data->color_b});
    light->SetDistance(data->distance);

    data.clear();

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
    data.make();
    
    glm::vec3 light_color;
    float light_distance;
    
    light_color = light->GetColor(); // idk if this actually works
    light_distance = light->GetDistance();
    
    data->color_r = light_color.r;
    data->color_g = light_color.g;
    data->color_b = light_color.b;
    data->distance = light_distance;
}

void Lamp::MessageHandler (Message& msg) {
    return;
}

}