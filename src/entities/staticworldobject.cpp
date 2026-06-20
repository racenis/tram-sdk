// Tramway Drifting and Dungeon Exploration Simulator SDK Runtime

#include <framework/worldcell.h>

#include <entities/staticworldobject.h>
#include <components/render.h>
#include <components/physics.h>

#include <framework/serialization.h>
#include <framework/settings.h>

/**
 * @class tram::StaticWorldObject entities/staticworldobject.h <entities/staticworldobject.h>
 * 
 * Worldspawn.
 * 
 * @see https://racenis.github.io/tram-sdk/documentation/entities/staticwobj.html
 */

namespace tram {
    
static Settings::Property<bool> STATIC_WOBJ_WIREFRAME = {false, "staticwobj-wireframe", Settings::NONE};
    
using namespace tram::Physics;

enum {
    FIELD_MODEL,
    FIELD_LIGHTMAP
};

static void toggle_wireframe(const char*) {
    Message msg {
        .type = STATIC_WOBJ_WIREFRAME ? Message::SET_FLAG_ON : Message::SET_FLAG_OFF,
        .sender = 0,
        .data_value = Message::AllocateData<Value>(name_t("wireframe"))
    };
    
    for (auto ent : Entity::GetAllOfType("staticwobj")) {
        msg.receiver = ent->GetID();
        Message::Send(msg);
    }
}

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
    Settings::SetCallback("staticwobj-wireframe", toggle_wireframe);
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
        
        rendercomponent->SetLineDrawingMode(STATIC_WOBJ_WIREFRAME);
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
    rendercomponent->SetModel(Render::Model::Find(model));
    rendercomponent->SetLightmap(lightmap ? Render::Lightmap::Find(lightmap) : Render::Lightmap::Find(id));
    rendercomponent->SetDirectionaLight(!cell->HasInteriorLighting());

    physicscomponent.make();
    physicscomponent->SetModel(Physics::CollisionModel::Find(model));
    physicscomponent->SetMass(0.0f);
    physicscomponent->SetParent(this);
    physicscomponent->SetCollisionGroup(COLL_WORLDOBJ);
    physicscomponent->SetCollisionMask(-1 ^ (COLL_WORLDOBJ | COLL_STATICOBJ));

    rendercomponent->Init();
    physicscomponent->Init();
    
    
    SetFlag(LOADED, true);

    UpdateParameters();
};

void StaticWorldObject::Unload(){
    SetFlag(LOADED, false);
    
    rendercomponent.clear();
    physicscomponent.clear();
};

void StaticWorldObject::Serialize(ValueArray& field_array) {
    field_array[FIELD_MODEL] = model;
    field_array[FIELD_LIGHTMAP] = lightmap;
};

void StaticWorldObject::MessageHandler(Message& msg) {
    switch (msg.type) {
        case Message::PING:
            Log(Severity::DEFAULT, System::MISC, "Entity {} : {} was pinged by {} : {}!", id, name, msg.sender, msg.sender ? Entity::Find(msg.sender)->GetName() : "none");
            break;
        case Message::SELECT:
            break;
        case Message::KILL:
            Yeet();
            break;
        case Message::SET_FLAG_ON:
        case Message::SET_FLAG_OFF: {
            const name_t flag = *msg.data_value;
            if (flag == "wireframe") {
                rendercomponent->SetLineDrawingMode(msg.type == Message::SET_FLAG_ON);
            } else {
                Log(Severity::WARNING, System::MISC, "Staticwobj does not have flag {}!", flag);
            }
        } break;
        
        default:
            Log(Severity::DEFAULT, System::MISC, "StaticWorldObject doesn't know how to handle {} message!", Message::GetName(msg.type));
    }
};

}