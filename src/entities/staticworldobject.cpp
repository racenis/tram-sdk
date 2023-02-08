// TRAMWAY DRIFT AND DUNGEON EXPLORATION SIMULATOR 2022
// All rights reserved.
//
// STATICWORLDOBJECT.CPP -- Static world object entity.

#include <framework/worldcell.h>

#include <entities/staticworldobject.h>
#include <components/rendercomponent.h>
#include <components/physicscomponent.h>

namespace Core {
    using namespace Core::Physics;

    StaticWorldObject::StaticWorldObject(std::string_view& str) : Entity(str) {
        std::cout << "Staticwobj constructor." << std::endl;

        data.make();
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
            physicscomponent->SetLocation(location);
            physicscomponent->SetRotation(rotation);
        }
    }

    void StaticWorldObject::Load(){
        rendercomponent.make();
        rendercomponent->SetModel(data->model);
        rendercomponent->SetLightmap(data->lightmap);
        rendercomponent->SetCellParams(cell->HasInteriorLighting());

        physicscomponent.make();
        physicscomponent->SetModel(data->model);
        physicscomponent->SetMass(0.0f);
        physicscomponent->SetParent(this);
        physicscomponent->SetCollisionGroup(COLL_WORLDOBJ);

        data.clear();

        rendercomponent->Init();
        physicscomponent->Init();
        isloaded = true;

        UpdateParameters();
    };

    void StaticWorldObject::Unload(){
        isloaded = false;
        Serialize();

        rendercomponent.clear();
        physicscomponent.clear();
    };

    void StaticWorldObject::Serialize() {
        data.make();

        data->model = rendercomponent->GetModel();
        data->lightmap = rendercomponent->GetLightmap();
    };

    void StaticWorldObject::MessageHandler(Message& msg){
        return;
    };

}
