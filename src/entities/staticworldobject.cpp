// TRAMWAY DRIFT AND DUNGEON EXPLORATION SIMULATOR 2022
// All rights reserved.

#include <framework/worldcell.h>

#include <entities/staticworldobject.h>
#include <components/rendercomponent.h>
#include <components/physicscomponent.h>

namespace tram {

using namespace tram::Physics;

StaticWorldObject::StaticWorldObject(std::string_view& str) : Entity(str) {
    data.make();
    data->FromString(str);
};

void StaticWorldObject::UpdateParameters() {
    if(is_loaded){
        rendercomponent->SetLocation(location);
        rendercomponent->SetRotation(rotation);
    }
}

void StaticWorldObject::SetParameters() {
    if(is_loaded){
        rendercomponent->SetLocation(location);
        rendercomponent->SetRotation(rotation);
        physicscomponent->SetLocation(location);
        physicscomponent->SetRotation(rotation);
    }
}

void StaticWorldObject::Load(){
    rendercomponent.make();
    rendercomponent->SetModel(data->model);
    rendercomponent->SetLightmap(data->lightmap);
    rendercomponent->SetWorldParameters(cell->HasInteriorLighting());

    physicscomponent.make();
    physicscomponent->SetModel(data->model);
    physicscomponent->SetMass(0.0f);
    physicscomponent->SetParent(this);
    physicscomponent->SetCollisionGroup(COLL_WORLDOBJ);

    data.clear();

    rendercomponent->Init();
    physicscomponent->Init();
    is_loaded = true;

    UpdateParameters();
};

void StaticWorldObject::Unload(){
    is_loaded = false;
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
