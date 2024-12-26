// Tramway Drifting and Dungeon Exploration Simulator SDK Runtime

#ifndef TRAM_SDK_EXTENSIONS_KITCHENSINK_DIALOG_H
#define TRAM_SDK_EXTENSIONS_KITCHENSINK_DIALOG_H

#include <framework/value.h>

#include <vector>

namespace tram::Ext::Kitchensink {

enum DialogType {
    DIALOG_TOPIC,
    DIALOG_IMPORT_SINGLE,
    DIALOG_IMPORT_MULTIPLE
    
    // TODO: add greetings, etc.
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

class DialogController {
public:
    DialogController(name_t initial_topic);
    
    name_t GetAnswer();
    std::vector<name_t> GetPrompts();
    
    bool ShouldExit();
    void MoveTo(size_t prompt_index);
private:
    name_t initial_topic;
    name_t current_topic;
    name_t answer;
    bool should_exit = false;
    std::vector<name_t> next_topics;
};

}

#endif // TRAM_SDK_EXTENSIONS_KITCHENSINK_DIALOG_H