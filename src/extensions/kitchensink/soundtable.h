// Tramway Drifting and Dungeon Exploration Simulator SDK Runtime


#ifndef TRAM_SDK_EXTENSIONS_DESIGN_SOUNDTABLE_H
#define TRAM_SDK_EXTENSIONS_DESIGN_SOUNDTABLE_H

#include <vector>
#include <framework/uid.h>
#include <framework/math.h>

namespace tram::Ext::Design {

enum SoundAction {
    SOUND_OPEN,
    SOUND_CLOSE,
    SOUND_LOCKED,
    SOUND_END_OPEN,
    SOUND_END_CLOSE,
    
    SOUND_WALK_ON,
    SOUND_JUMP_ON,
    SOUND_HIT,
};

class SoundTable {
public:
    void AddSound(SoundAction type, name_t sound);
    name_t GetSound(SoundAction type);
    void PlaySound(SoundAction type);
    void PlaySound(SoundAction type, vec3 position, float volume);
    
    static SoundTable* Find(name_t name);
private:
    std::vector<std::pair<SoundAction, name_t>> sounds;
};

}

#endif // TRAM_SDK_EXTENSIONS_DESIGN_SOUNDTABLE_H