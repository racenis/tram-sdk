// Tramway Drifting and Dungeon Exploration Simulator SDK Runtime

#ifndef TRAM_SDK_ENTITIES_MARKER_H
#define TRAM_SDK_ENTITIES_MARKER_H

#include <framework/entity.h>
#include <framework/serialization.h>

namespace tram {
    
class Marker : public Entity {
public:
    Marker(const SharedEntityData&, const ValueArray&);

    void UpdateParameters();
    void SetParameters();
    void Load();
    void Unload();
    void Serialize();
    void MessageHandler(Message& msg);
    name_t GetType();
    static void Register();
};

}

#endif // TRAM_SDK_ENTITIES_MARKER_H