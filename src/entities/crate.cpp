// TRAMWAY DRIFT AND DUNGEON EXPLORATION SIMULATOR 2022
// All rights reserved.
//
// CRATE.CPP -- Crate entity.

#include <framework/worldcell.h>

#include <entities/crate.h>

#include <components/rendercomponent.h>
#include <components/physicscomponent.h>

namespace Core {
    using namespace Core::Physics;
    
    Crate::Crate(std::string_view& str) : Entity(str) {
        serializeddata.make();
        serializeddata->FromString(str);
    }

    Crate::Crate(const char* nname, const char* modelname, const char* collisionmodelname, glm::vec3 pos, glm::vec3 rot){
        name = UID(nname);
        location = pos;
        rotation = glm::quat(rot);

        //serialized_data = new Data();
        //auto data = (Data*) serialized_data;
        serializeddata.make();
        serializeddata->model = UID(modelname);
        serializeddata->collmodel = UID(collisionmodelname);
    }

    void Crate::UpdateParameters() {
        if (!isloaded) return;
        rendercomponent->UpdateLocation(location);
        rendercomponent->UpdateRotation(rotation);
        rendercomponent->SetCellParams(cell->HasInteriorLighting());
    }
    
    void Crate::SetParameters() {
        if (!isloaded) return;
        UpdateParameters();
        physicscomponent->SetLocation(location);
        physicscomponent->SetRotation(rotation);
    }

    void Crate::Load(){
        //auto data = (Data*) serialized_data;

        rendercomponent.make();
        physicscomponent.make();
        
        rendercomponent->SetModel((name_t)serializeddata->model);
        rendercomponent->SetPose(nullptr);

        physicscomponent->SetParent(this);
        physicscomponent->SetCollisionGroup(COLL_DYNAMICOBJ);
        physicscomponent->SetModel(serializeddata->collmodel);
        physicscomponent->SetMass(68.9f);
        physicscomponent->SetSleep(true);
        

        //delete serialized_data;
        //serialized_data = nullptr;
        serializeddata.clear();

        rendercomponent->Init();
        physicscomponent->Init();
        isloaded = true;

        UpdateParameters();
    }

    void Crate::Unload() {
        isloaded = false;

        Serialize();

        rendercomponent.clear();
        physicscomponent.clear();
    }

    void Crate::Serialize() {
        //serialized_data = new Data();
        //auto data = (Data*) serialized_data;
        serializeddata.make();

        serializeddata->model = rendercomponent->GetModel();
        serializeddata->collmodel = physicscomponent->GetModel();
    }

    void Crate::MessageHandler(Message& msg){
        //if (msg.type == Message::ACTIVATE) Activate(msg);
        return;
    }
    
    void Crate::Testingolingo() {
        assert(isloaded);
        assert(physicscomponent);
        physicscomponent->DisableRotation();
        physicscomponent->SetRotation(glm::vec3(0.0f, 0.785f, 0.0f));
    }
}
