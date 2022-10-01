// TRAMWAY DRIFT AND DUNGEON EXPLORATION SIMULATOR 2022
// All rights reserved.
//
// LAMP.CPP -- Lamp entity.

#include <core.h>
#include <entities/lamp.h>
#include <components/lightcomponent.h>

namespace Core {
    Lamp::Lamp(std::string_view& str){
        Entity::SetParameters(str);

        serialized_data = new Data();
        auto data = (Data*) serialized_data;

        data->FromString(str);
    }
    
    void Lamp::UpdateParameters() {
        if (!isloaded) return;
        light->UpdateLocation(location);
    }
    
    void Lamp::SetParameters() {
        UpdateParameters();
    }
    
    void Lamp::Load(){
        auto data = (Data*) serialized_data;

        light = PoolProxy<LightComponent>::New();
        light->UpdateColor(glm::vec3(data->color_r, data->color_g, data->color_b));
        light->UpdateDistance(data->distance);

        delete serialized_data;
        serialized_data = nullptr;

        light->Init();
        isloaded = true;

        UpdateParameters();
    }
    
    void Lamp::Unload(){
        isloaded = false;

        Serialize();

        light->Uninit();

        PoolProxy<LightComponent>::Delete(light);
        light = nullptr;
    }

    void Lamp::Serialize(){
        serialized_data = new Data();
        auto data = (Data*) serialized_data;
        
        glm::vec3 light_color;
        float light_distance;
        
        light->GetColor(light_color); // idk if this actually works
        light->GetDistance(light_distance);
        
        data->color_r = light_color.r;
        data->color_g = light_color.g;
        data->color_b = light_color.b;
        data->distance = light_distance;
    }

    void Lamp::MessageHandler(Message& msg){
        return;
    }
}