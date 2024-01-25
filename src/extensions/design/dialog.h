// TRAMWAY DRIFT AND DUNGEON EXPLORATION SIMULATOR 2022
// All rights reserved.

#ifndef TRAM_SDK_EXTENSIONS_DESIGN_DIALOG_H
#define TRAM_SDK_EXTENSIONS_DESIGN_DIALOG_H

#include <framework/value.h>

#include <extensions/design/inventory.h>

namespace tram::Ext::Design {

struct DialogCondition {
    enum Type {
        QUEST_STAGE_MORETHAN,
        QUEST_STAGE_LESSTHAN,
        PLAYER_HAS_ITEM,
        ENTITY_QUERY,
    };
    
    Type type;
    
    name_t quest;
    name_t stage;
    
    name_t query;
    Value value;
};

struct DialogAction {
    enum Type {
        QUEST_SET_STAGE,
        QUEST_ADVANCE_STAGE,
        PLAYER_GIVE_ITEM,
        PLAYER_TAKE_ITEM,
    };
    
    Type type;
    
    name_t quest;
    name_t stage;
    name_t item_class;
    int item_count;
    
    name_t message;
    Value value;
};
    
struct DialogTopic {
    name_t name;
    
    name_t prompt;
    name_t answer;

    std::vector<DialogCondition> conditions;
    std::vector<DialogAction> actions;

    std::vector<name_t> next_topics;
    
    void Register(const DialogTopic& topic);
    const DialogTopic& Find(name_t topic);
};
    

}

#endif // TRAM_SDK_EXTENSIONS_DESIGN_DIALOG_H