// TRAMWAY DRIFT AND DUNGEON EXPLORATION SIMULATOR 2022
// All rights reserved.

#include <extensions/kitchensink/soundtable.h>

#include <render/render.h>
#include <entities/sound.h>

#include <templates/hashmap.h>
#include <templates/pool.h>

template <> tram::Pool<tram::Ext::Design::SoundTable> tram::PoolProxy<tram::Ext::Design::SoundTable>::pool("sountbale pool", 100);

namespace tram::Ext::Design {

static Hashmap<SoundTable*> sound_table_list("soundtbale name list", 200);
    
void SoundTable::AddSound(SoundAction type, name_t sound) {
    this->sounds.push_back({type, sound});
}

name_t SoundTable::GetSound(SoundAction type) {
    std::vector<name_t> candidates;
    
    for (const auto& entry : this->sounds) {
        if (entry.first == type) candidates.push_back(entry.second);
    }
    
    if (candidates.empty()) return "none";
    
    return candidates[rand() % candidates.size()];
}

void SoundTable::PlaySound(SoundAction type) {
    this->PlaySound(type, Render::GetViewPosition(), 1.0f);
}

void SoundTable::PlaySound(SoundAction type, vec3 position, float volume) {
    name_t sound = GetSound(type);
    if (!sound) return;
    new Sound(sound, volume, position);
}



SoundTable* SoundTable::Find(name_t name) {
    SoundTable* sound_table = sound_table_list.Find(name);
    
    if (!sound_table) {
        sound_table = PoolProxy<SoundTable>::New();
        sound_table_list.Insert(name, sound_table);
    }
    
    return sound_table;
}
    

}
