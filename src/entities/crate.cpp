// Tramway Drifting and Dungeon Exploration Simulator SDK Runtime


#include <framework/worldcell.h>

#include <entities/crate.h>
#include <entities/script.h>

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

void Crate::Register() {
    Entity::RegisterType(
        "crate", 
        [](const SharedEntityData& a, const ValueArray& b) -> Entity* { return new Crate(a, b); },
        [](Entity* a) { delete a; },
        {
            {FIELD_MODEL,       TYPE_NAME,      FIELD_SERIALIZE},
            {FIELD_COLLMODEL,   TYPE_NAME,      FIELD_SERIALIZE}
        }
    );
    
    ScriptableType::Register(
        "crate",
        [](name_t type, const SharedEntityData& a, const ValueArray& b) -> Entity* { return new Scriptable<Crate>(a, b, type); },
        [](Entity* a) { delete a; }
    );
}

name_t Crate::GetType() {
    return "crate";
}

Crate::Crate(const SharedEntityData& shared_data, const ValueArray& field_array) : Entity(shared_data) {
    model = field_array[FIELD_MODEL];
    collmodel = field_array[FIELD_COLLMODEL];
}

Crate::Crate (const char* nname, const char* modelname, const char* collisionmodelname, vec3 pos, vec3 rot) : Entity(nname) {
    name = UID(nname);
    location = pos;
    rotation = quat(rot);

    model = UID(modelname);
    collmodel = UID(collisionmodelname);
}

void Crate::UpdateParameters() {
    if (!IsLoaded()) return;
    rendercomponent->SetLocation(location);
    rendercomponent->SetRotation(rotation);
    if (cell) {
        rendercomponent->SetDirectionaLight(!cell->HasInteriorLighting());
    } else {
        rendercomponent->SetDirectionaLight(true);
    }
}

void Crate::SetParameters() {
    if (!IsLoaded()) return;
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
    flags |= LOADED;

    UpdateParameters();
}

void Crate::Unload() {
    flags &= ~LOADED;

    Serialize();

    rendercomponent.clear();
    physicscomponent.clear();
}

void Crate::Serialize() {
    model = rendercomponent->GetModel()->GetName();
    collmodel = physicscomponent->GetModel();
}

void Crate::MessageHandler(Message& msg){
    if (msg.type != Message::MOVE_PICK_UP) {
        return;
    }
}

void Crate::Testingolingo() {
    assert(IsLoaded());
    assert(physicscomponent);
    physicscomponent->SetAngularFactor({0.0f, 0.0f, 0.0f});
    physicscomponent->SetRotation(vec3(0.0f, 0.785f, 0.0f));
}

}
