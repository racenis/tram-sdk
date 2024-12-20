// TRAMWAY DRIFT AND DUNGEON EXPLORATION SIMULATOR 2022
// All rights reserved.

#ifndef TRAM_SDK_EXTENSIONS_DESIGN_DIALOG_H
#define TRAM_SDK_EXTENSIONS_DESIGN_DIALOG_H

#include <framework/value.h>

#include <vector>

namespace tram::Ext::Kitchensink {

enum DialogType {
    DIALOG_TOPIC,
    DIALOG_IMPORT_SINGLE,
    DIALOG_IMPORT_MULTIPLE
};  

struct DialogCondition {
    name_t quest;
    name_t variable;
    
    bool IsMet();
};

struct DialogAction {
    name_t quest;
    name_t trigger;
    
    void Perform();
};

struct DialogTopic {
    DialogType type = DIALOG_TOPIC;
    
    name_t name;
    
    name_t prompt;
    name_t answer;

    DialogCondition condition;
    DialogAction action;

    std::vector<name_t> next_topics;
    
    std::vector<name_t> GetValidNextTopics();
    void Gather(std::vector<name_t>& topics);
    
    static void LoadFromDisk(const char* filename);
    
    static DialogTopic* Make(name_t);
    static DialogTopic* Find(name_t);
};

}

#endif // TRAM_SDK_EXTENSIONS_DESIGN_DIALOG_H