// TRAMWAY DRIFT AND DUNGEON EXPLORATION SIMULATOR 2022
// All rights reserved.

#include <extensions/kitchensink/ai.h>

#include <render/render.h>
#include <entities/sound.h>

#include <templates/hashmap.h>
#include <templates/pool.h>

template <> tram::Pool<tram::Ext::Kitchensink::AIPackage> tram::PoolProxy<tram::Ext::Kitchensink::AIPackage>::pool("animatio pool", 100);

namespace tram::Ext::Kitchensink {

static Hashmap<AIPackage*> sound_table_list("soundtbale name list", 200);

AIPackage* AIPackage::Find(name_t name) {
    AIPackage* sound_table = sound_table_list.Find(name);
    
    if (!sound_table) {
        sound_table = PoolProxy<AIPackage>::New();
        sound_table_list.Insert(name, sound_table);
    }
    
    return sound_table;
}
    

}
