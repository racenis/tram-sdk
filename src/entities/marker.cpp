#include <entities/marker.h>

namespace Core {
    
Marker::Marker(std::string_view& str) : Entity(str) {}

void Marker::UpdateParameters() {}
void Marker::SetParameters() {}
void Marker::Load() {}
void Marker::Unload() {}
void Marker::Serialize() {}
void Marker::MessageHandler(Message& msg) {}
    
}