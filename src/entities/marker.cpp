// TRAMWAY DRIFT AND DUNGEON EXPLORATION SIMULATOR 2022
// All rights reserved.

#include <entities/marker.h>

namespace tram {
    
Marker::Marker(const SharedEntityData& shared_data, const ValueArray& field_array) : Entity(shared_data) {}

void Marker::Register() {
    Entity::RegisterType(
        "marker", 
        [](const SharedEntityData& a, const ValueArray& b) -> Entity* { return new Marker(a, b); },
        [](Entity* a) { delete a; },
        nullptr,
        0
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