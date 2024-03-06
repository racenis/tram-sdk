// TRAMWAY DRIFT AND DUNGEON EXPLORATION SIMULATOR 2022
// All rights reserved.

#ifndef TRAM_SDK_ENTITIES_AUDIO_H
#define TRAM_SDK_ENTITIES_AUDIO_H

#include <framework/entity.h>
#include <framework/entitycomponent.h>
#include <framework/serialization.h>

namespace tram {

class AudioComponent;

class Sound : public Entity {
public:
    Sound(const SharedEntityData&, const ValueArray&);
    Sound(name_t sound, float volume, vec3 position);
    void UpdateParameters();
    void SetParameters();
    void Load();
    void Unload();
    void Serialize();
    void MessageHandler(Message& msg);
    static void Register();
protected:
    Component<AudioComponent> audio;
    
    name_t sound;
    float volume;
    int flags;
};

}

#endif // TRAM_SDK_ENTITIES_AUDIO_H