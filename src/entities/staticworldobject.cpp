// TRAMWAY DRIFT AND DUNGEON EXPLORATION SIMULATOR 2022
// All rights reserved.
//
// STATICWORLDOBJECT.CPP -- Static world object entity.

#include <entities/staticworldobject.h>
#include <components/rendercomponent.h>
#include <components/physicscomponent.h>

namespace Core {
    using namespace Core::Physics;

    StaticWorldObject::StaticWorldObject(std::string_view& str) : Entity(str) {
        std::cout << "Staticwobj constructor." << std::endl;

        serialized_data = new Data();
        auto data = (Data*) serialized_data;

        data->FromString(str);
    };

    void StaticWorldObject::UpdateParameters() {
        if(isloaded){
            rendercomponent->UpdateLocation(location);
            rendercomponent->UpdateRotation(rotation);
        }
    }
    
    void StaticWorldObject::SetParameters() {
        if(isloaded){
            rendercomponent->UpdateLocation(location);
            rendercomponent->UpdateRotation(rotation);
            physicscomponent->UpdateLocation(location);
            physicscomponent->UpdateRotation(rotation);
        }
    }

    void StaticWorldObject::Load(){
        Data* data = (Data*) serialized_data;

        rendercomponent = PoolProxy<RenderComponent>::New();
        rendercomponent->SetModel(data->model);
        rendercomponent->SetLightmap(data->lightmap);
        rendercomponent->SetCellParams(cell->HasInteriorLighting());

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
