// TRAMWAY DRIFT AND DUNGEON EXPLORATION SIMULATOR 2022
// All rights reserved.

#include <entities/marker.h>

namespace tram {
    
Marker::Marker(std::string_view& str) : Entity(str) {}

void Marker::UpdateParameters() {}
void Marker::SetParameters() {}
void Marker::Load() {}
void Marker::Unload() {}
void Marker::Serialize() {}
void Marker::MessageHandler(Message& msg) {}
    
}