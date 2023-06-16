// TRAMWAY DRIFT AND DUNGEON EXPLORATION SIMULATOR 2022
// All rights reserved.

#include <framework/worldcell.h>

#include <entities/staticworldobject.h>
#include <components/render.h>
#include <components/physics.h>

#include <framework/serialization.h>

namespace tram {

using namespace tram::Physics;

enum {
    FIELD_MODEL,
    FIELD_LIGHTMAP
};

static const uint32_t fields[2] = {
    TYPE_NAME,
    TYPE_NAME
}; 

void StaticWorldObject::Register() {
    Entity::RegisterType(
        "staticwobj", 
        [](const SharedEntityData& a, const SerializedFieldArray& b) -> Entity* { return new StaticWorldObject(a, b); },
        [](Entity* a) { delete a; },
        fields,
        2
    );
}

StaticWorldObject::StaticWorldObject(const SharedEntityData& shared_data, const SerializedFieldArray& field_array) : Entity(shared_data) {
    model = field_array[FIELD_MODEL];
    lightmap = field_array[FIELD_LIGHTMAP];
}

void StaticWorldObject::UpdateParameters() {
    if (is_loaded) {
        rendercomponent->SetLocation(location);
        rendercomponent->SetRotation(rotation);
    }
}

void StaticWorldObject::SetParameters() {
    if (is_loaded) {
        rendercomponent->SetLocation(location);
        rendercomponent->SetRotation(rotation);
        physicscomponent->SetLocation(location);
        physicscomponent->SetRotation(rotation);
    }
}

void StaticWorldObject::Load(){
    rendercomponent.make();
    rendercomponent->SetParent(this);
    rendercomponent->SetModel(model);
    rendercomponent->SetLightmap(lightmap);
    rendercomponent->SetWorldParameters(cell->HasInteriorLighting());

    physicscomponent.make();
    physicscomponent->SetModel(model);
    physicscomponent->SetMass(0.0f);
    physicscomponent->SetParent(this);
    physicscomponent->SetCollisionGroup(COLL_WORLDOBJ);

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
    model = rendercomponent->GetModel();
    lightmap = rendercomponent->GetLightmap();
};

void StaticWorldObject::MessageHandler(Message& msg){
    return;
};

}
