// Tramway Drifting and Dungeon Exploration Simulator SDK Runtime


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

void StaticWorldObject::Register() {
    Entity::RegisterType(
        "staticwobj", 
        [](const SharedEntityData& a, const ValueArray& b) -> Entity* { return new StaticWorldObject(a, b); },
        [](Entity* a) { delete a; },
        {
            {FIELD_MODEL,       TYPE_NAME,      FIELD_SERIALIZE},
            {FIELD_LIGHTMAP,    TYPE_NAME,      FIELD_SERIALIZE}
        }
    );
}

name_t StaticWorldObject::GetType() {
    return "staticwobj";
}

StaticWorldObject::StaticWorldObject(const SharedEntityData& shared_data, const ValueArray& field_array) : Entity(shared_data) {
    model = field_array[FIELD_MODEL];
    lightmap = field_array[FIELD_LIGHTMAP];
}

void StaticWorldObject::UpdateParameters() {
    if (IsLoaded()) {
        rendercomponent->SetLocation(location);
        rendercomponent->SetRotation(rotation);
    }
}

void StaticWorldObject::SetParameters() {
    if (IsLoaded()) {
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
    rendercomponent->SetDirectionaLight(!cell->HasInteriorLighting());

    physicscomponent.make();
    physicscomponent->SetModel(model);
    physicscomponent->SetMass(0.0f);
    physicscomponent->SetParent(this);
    physicscomponent->SetCollisionGroup(COLL_WORLDOBJ);
    physicscomponent->SetCollisionMask(-1 ^ (COLL_WORLDOBJ | COLL_STATICOBJ));

    rendercomponent->Init();
    physicscomponent->Init();
    flags |= LOADED;

    UpdateParameters();
};

void StaticWorldObject::Unload(){
    flags &= ~LOADED;
    Serialize();

    rendercomponent.clear();
    physicscomponent.clear();
};

void StaticWorldObject::Serialize() {
    model = rendercomponent->GetModel()->GetName();
    lightmap = rendercomponent->GetLightmap();
};

void StaticWorldObject::MessageHandler(Message& msg){
    return;
};

}
