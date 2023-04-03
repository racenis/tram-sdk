// TRAMWAY DRIFT AND DUNGEON EXPLORATION SIMULATOR 2022
// All rights reserved.

#include <framework/worldcell.h>

#include <entities/crate.h>

#include <components/rendercomponent.h>
#include <components/physicscomponent.h>

namespace tram {

using namespace tram::Physics;

Crate::Crate(std::string_view& str) : Entity(str) {
    serializeddata.make();
    serializeddata->FromString(str);
}

Crate::Crate (const char* nname, const char* modelname, const char* collisionmodelname, glm::vec3 pos, glm::vec3 rot) : Entity(nname) {
    name = UID(nname);
    location = pos;
    rotation = glm::quat(rot);

    serializeddata.make();
    serializeddata->model = UID(modelname);
    serializeddata->collmodel = UID(collisionmodelname);
}

void Crate::UpdateParameters() {
    if (!is_loaded) return;
    rendercomponent->SetLocation(location);
    rendercomponent->SetRotation(rotation);
    rendercomponent->SetWorldParameters(cell->HasInteriorLighting());
}

void Crate::SetParameters() {
    if (!is_loaded) return;
    UpdateParameters();
    physicscomponent->SetLocation(location);
    physicscomponent->SetRotation(rotation);
}

void Crate::Load(){
    rendercomponent.make();
    physicscomponent.make();
    
    rendercomponent->SetModel(serializeddata->model);

    physicscomponent->SetParent(this);
    physicscomponent->SetCollisionGroup(COLL_DYNAMICOBJ);
    physicscomponent->SetModel(serializeddata->collmodel);
    physicscomponent->SetMass(68.9f);
    physicscomponent->SetSleep(true);
    
    serializeddata.clear();

    rendercomponent->Init();
    physicscomponent->Init();
    is_loaded = true;

    UpdateParameters();
}

void Crate::Unload() {
    is_loaded = false;

    Serialize();

    rendercomponent.clear();
    physicscomponent.clear();
}

void Crate::Serialize() {
    serializeddata.make();

    serializeddata->model = rendercomponent->GetModel();
    serializeddata->collmodel = physicscomponent->GetModel();
}

void Crate::MessageHandler(Message& msg){
    return;
}

void Crate::Testingolingo() {
    assert(is_loaded);
    assert(physicscomponent);
    physicscomponent->SetAngularFactor({0.0f, 0.0f, 0.0f});
    physicscomponent->SetRotation(glm::vec3(0.0f, 0.785f, 0.0f));
}

}
