// TRAMWAY DRIFT AND DUNGEON EXPLORATION SIMULATOR 2022
// All rights reserved.

#include <framework/worldcell.h>

#include <entities/crate.h>

#include <components/render.h>
#include <components/physics.h>

#include <framework/serialization.h>
#include <framework/message.h>

namespace tram {

using namespace tram::Physics;

enum {
    FIELD_MODEL,
    FIELD_COLLMODEL
};

static const uint32_t fields[2] = {
    TYPE_NAME,
    TYPE_NAME
}; 

void Crate::Register() {
    Entity::RegisterType(
        "crate", 
        [](const SharedEntityData& a, const SerializedFieldArray& b) -> Entity* { return new Crate(a, b); },
        [](Entity* a) { delete a; },
        fields,
        2
    );
}

Crate::Crate(const SharedEntityData& shared_data, const SerializedFieldArray& field_array) : Entity(shared_data) {
    model = field_array[FIELD_MODEL];
    collmodel = field_array[FIELD_COLLMODEL];
}

Crate::Crate (const char* nname, const char* modelname, const char* collisionmodelname, glm::vec3 pos, glm::vec3 rot) : Entity(nname) {
    name = UID(nname);
    location = pos;
    rotation = glm::quat(rot);

    model = UID(modelname);
    collmodel = UID(collisionmodelname);
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
    
    rendercomponent->SetParent(this);
    rendercomponent->SetModel(model);

    physicscomponent->SetParent(this);
    physicscomponent->SetCollisionGroup(COLL_DYNAMICOBJ);
    physicscomponent->SetModel(collmodel);
    physicscomponent->SetMass(68.9f);
    physicscomponent->Sleep();
    
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
    model = rendercomponent->GetModel();
    collmodel = physicscomponent->GetModel();
}

void Crate::MessageHandler(Message& msg){
    if (msg.type != Message::MOVE_PICK_UP) {
        return;
    }
    
    holder = msg.sender;
    
    if (msg.data) {
        physicscomponent->Awaken();
        physicscomponent->SetKinematic(true);
        //physicscomponent->SetLocation(*(vec3*)msg.data);
        //rendercomponent->SetLocation(*(vec3*)msg.data);
        this->location = *(vec3*)msg.data;
        UpdateParameters();
    } else {
        physicscomponent->SetKinematic(false);
        UpdateParameters();
    }

    
}

void Crate::Testingolingo() {
    assert(is_loaded);
    assert(physicscomponent);
    physicscomponent->SetAngularFactor({0.0f, 0.0f, 0.0f});
    physicscomponent->SetRotation(glm::vec3(0.0f, 0.785f, 0.0f));
}

}
