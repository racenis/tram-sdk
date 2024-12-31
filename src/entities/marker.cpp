// Tramway Drifting and Dungeon Exploration Simulator SDK Runtime

#include <entities/marker.h>
#include <entities/script.h>

/**
 * @class tram::Marker entities/marker.h <entities/marker.h>
 * 
 * Functionally inert.
 * Doesn't do anything and has no functionality. Can be used as a template.
 * 
 * @see https://racenis.github.io/tram-sdk/documentation/entities/marker.html
 */

namespace tram {
    
Marker::Marker(const SharedEntityData& shared_data, const ValueArray& field_array) : Entity(shared_data) {}

void Marker::Register() {
    Entity::RegisterType(
        "marker", 
        [](const SharedEntityData& a, const ValueArray& b) -> Entity* { return new Marker(a, b); },
        [](Entity* a) { delete a; },
        (FieldInfo*)nullptr,
        0
    );
    
    ScriptableType::Register(
        "marker",
        [](name_t type, const SharedEntityData& a, const ValueArray& b) -> Entity* { return new Scriptable<Marker>(a, b, type); },
        [](Entity* a) { delete a; }
    );
}

name_t Marker::GetType() {
    return "marker";
}

void Marker::UpdateParameters() {}
void Marker::SetParameters() {}
void Marker::Load() {}
void Marker::Unload() {}
void Marker::Serialize() {}
void Marker::MessageHandler(Message& msg) {}

}