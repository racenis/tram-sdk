// TRAMWAY DRIFT AND DUNGEON EXPLORATION SIMULATOR 2022
// All rights reserved.
//
// STATICWORLDOBJECT.CPP -- Static world object entity.

#include <core.h>
#include <entities.h>
#include <physics.h>

namespace Core {
    using namespace Core::Physics;

    StaticWorldObject::StaticWorldObject(std::string_view& str){
        SetParameters(str);

        serialized_data = new Data();
        auto data = (Data*) serialized_data;

        data->FromString(str);
    };

    void StaticWorldObject::UpdateParameters() {
        if(isloaded){
            rendercomponent->UpdateLocation(location);
            rendercomponent->UpdateRotation(rotation);
        }
    };

    void StaticWorldObject::Load(){
        Data* data = (Data*) serialized_data;

        rendercomponent = PoolProxy<RenderComponent>::New();
        rendercomponent->SetModel(data->model);
        rendercomponent->SetLightmap(data->lightmap);

        physicscomponent = PoolProxy<PhysicsComponent>::New();
        physicscomponent->SetModel(data->model);
        physicscomponent->SetMass(0.0f);
        physicscomponent->SetParent(this);
        physicscomponent->SetCollisionGroup(COLL_WORLDOBJ);

        delete serialized_data;
        serialized_data = nullptr;

        rendercomponent->Init();
        physicscomponent->Init();
        isloaded = true;

        UpdateParameters();
    };

    void StaticWorldObject::Unload(){
        isloaded = false;
        Serialize();

        rendercomponent->Uninit();
        physicscomponent->Uninit();

        PoolProxy<RenderComponent>::Delete(rendercomponent);
        rendercomponent = nullptr;
        PoolProxy<PhysicsComponent>::Delete(physicscomponent);
        physicscomponent = nullptr;
    };

    void StaticWorldObject::Serialize() {
        serialized_data = new Data();
        auto data = (Data*) serialized_data;

        data->model = rendercomponent->GetModel();
        data->lightmap = rendercomponent->GetLightmap();
    };

    void StaticWorldObject::MessageHandler(Message& msg){
        return;
    };

}
