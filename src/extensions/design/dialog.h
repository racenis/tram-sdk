// TRAMWAY DRIFT AND DUNGEON EXPLORATION SIMULATOR 2022
// All rights reserved.

#ifndef TRAM_SDK_EXTENSIONS_DESIGN_DIALOG_H
#define TRAM_SDK_EXTENSIONS_DESIGN_DIALOG_H

#include <extensions/design/inventory.h>

namespace tram::Ext::Design {
    
struct DialogTopic {
    name_t name;
    
    name_t prompt;
    name_t answer;
    
    name_t quest;
    int stage_enable = -1;
    int stage_disable = -1;
    
    int set_stage = -1;
    
    name_t quest_script;
    
    item_class item;
    int count = 1;
    
    name_t next_topics[6];
    
    void Register(const DialogTopic& topic);
    const DialogTopic& Find(name_t topic);
};
    

}

#endif // TRAM_SDK_EXTENSIONS_DESIGN_DIALOG_H