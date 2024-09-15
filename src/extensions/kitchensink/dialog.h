// TRAMWAY DRIFT AND DUNGEON EXPLORATION SIMULATOR 2022
// All rights reserved.

#ifndef TRAM_SDK_EXTENSIONS_DESIGN_DIALOG_H
#define TRAM_SDK_EXTENSIONS_DESIGN_DIALOG_H

#include <framework/value.h>

#include <vector>

namespace tram::Ext::Kitchensink {
    
struct DialogTopic {
    name_t name;
    
    name_t prompt;
    name_t answer;

    name_t condition_quest;
    name_t condition_variable;
    
    name_t action_quest;
    name_t action_variable;

    std::vector<name_t> next_topics;
    
    bool ConditionMet();
    void PerformAction();
    
    static DialogTopic* Make(name_t);
    static DialogTopic* Find(name_t);
};
    

}

#endif // TRAM_SDK_EXTENSIONS_DESIGN_DIALOG_H