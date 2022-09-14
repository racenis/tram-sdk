// TRAMWAY DRIFT AND DUNGEON EXPLORATION SIMULATOR 2022
// All rights reserved.
//
// LAMP.CPP -- Lamp entity.

#include <core.h>
#include <entities/entities.h>

#ifdef BEGONIS

Lamp::Lamp(std::string_view& str){
        SetParameters(str);

        serializeddata = entityData.AddNew();
        Data* data = (Data*) serializeddata;

        data->color[0] = PFloat(str);
        data->color[1] = PFloat(str);
        data->color[2] = PFloat(str);
        data->distance = PFloat(str);
    };
    void Lamp::UpdateParameters() {
        if(isloaded) light->UpdateLocation(location.x, location.y, location.z);
    };
    void Lamp::Load(){
        std::cout << "Loading light: " << ReverseUID(name) << std::endl;
        Data* data = (Data*) serializeddata;
        light = lightCompPool.AddNew();

        light->UpdateColor(data->color[0], data->color[1], data->color[2]);
        light->UpdateDistance(data->distance);
        light->Init();

        entityData.Remove(serializeddata);
        serializeddata = nullptr;

        isloaded = true;

        UpdateParameters();
    }
    void Lamp::Unload(){

        isloaded = false;

        Serialize();

        light->Uninit();

        lightCompPool.Remove(light);
        light = nullptr;
    }

    void Lamp::Serialize(){
        serializeddata = entityData.AddNew();
        Data* data = (Data*) serializeddata;

        light->GetColor(data->color);   // idk if this actually works
        light->GetDistance(data->distance);
    }

    void Lamp::SerializeString(std::string& str){
        Data* data = (Data*) serializeddata;
        str = "lamp ";
        ParametersString(str);
        str.append(std::to_string(data->color[0]));
        str.push_back(' ');
        str.append(std::to_string(data->color[1]));
        str.push_back(' ');
        str.append(std::to_string(data->color[2]));
        str.push_back(' ');
        str.append(std::to_string(data->distance));
    }

    void Lamp::MessageHandler(Message& msg){
        return;
    }

#endif // BEGONIS