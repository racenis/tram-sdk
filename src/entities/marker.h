// TRAMWAY DRIFT AND DUNGEON EXPLORATION SIMULATOR 2022
// All rights reserved.

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
    static void Register();
};

}

#endif // TRAM_SDK_ENTITIES_MARKER_H