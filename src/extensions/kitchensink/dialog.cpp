// Tramway Drifting and Dungeon Exploration Simulator SDK Runtime

#include <extensions/kitchensink/dialog.h>
#include <extensions/kitchensink/quest.h>
#include <extensions/kitchensink/kitchensink.h>

#include <templates/pool.h>
#include <templates/hashmap.h>

#include <framework/file.h>

#include <config.h>

using namespace tram;
template <> Pool<Ext::Kitchensink::DialogTopic> PoolProxy<Ext::Kitchensink::DialogTopic>::pool("DialogTopic pool", 500);
static Hashmap<Ext::Kitchensink::DialogTopic*> item_class_list("DialogTopic list", 500);

namespace tram::Ext::Kitchensink {

bool DialogCondition::IsMet() {
    if (!quest) return true;
    Value cond = Quest::Find(quest)->GetVariable(variable);
    assert(cond.GetType() == TYPE_BOOL);
    return (bool)cond;
}

void DialogAction::Perform() {
    if (!quest) return;
    Quest::Find(quest)->FireTrigger(trigger);
}

std::vector<name_t> DialogTopic::GetValidNextTopics() {
    std::vector<name_t> topics;
    
    for (auto topic_name : next_topics) {
        DialogTopic* topic = DialogTopic::Find(topic_name);
       
        if (!topic) {
            topics.push_back(topic_name);
            continue;
        }
        
        if (!topic->condition.IsMet()) continue;
       
        topic->Gather(topics);
    }
    
    return topics;
}

void DialogTopic::Gather(std::vector<name_t>& topics) {
    switch (type) {
        case DIALOG_TOPIC:
            topics.push_back(name);
            break;
        case DIALOG_IMPORT_SINGLE:
        case DIALOG_IMPORT_MULTIPLE:
            for (auto topic_name : next_topics) {
                DialogTopic* topic = DialogTopic::Find(topic_name);
               
                if (!topic) {
                    topics.push_back(topic_name);
                    continue;
                }
                
                if (!topic->condition.IsMet()) continue;
                
                topic->Gather(topics);
                
                if (type == DIALOG_IMPORT_SINGLE) break;
            }
    }
}

DialogTopic* DialogTopic::Make(name_t name) {
    DialogTopic* topic = item_class_list.find(name);
    
    if (!topic) {
        topic = PoolProxy<DialogTopic>::make();
        topic->name = name;
        item_class_list.insert(name, topic);
    }
    
    return topic;
}

DialogTopic* DialogTopic::Find(name_t name) {
    return item_class_list.find(name);
}

void DialogTopic::LoadFromDisk(const char* filename) {
    char path[PATH_LIMIT];
    snprintf(path, PATH_LIMIT, "data/%s.dialog", filename);

    File file(path, File::READ | File::PAUSE_LINE);

    if (!file.is_open()) {
        Log(Severity::NOTE, Kitchensink::System(), "Can't open dialog file: {}", path);
        return;
    }

    name_t file_type = file.read_name(); file.skip_linebreak();

    if (file_type != "DIALOGv1") {
        Log(Severity::WARNING, Kitchensink::System(), "Invalid file type '{}' in dialog file: {}", file_type, path);
        return;
    }
    
    Log(Severity::INFO, Kitchensink::System(), "Loading dialog: {}", path);

    while (file.is_continue()) {
        auto record = file.read_name();
        
        if (record == "dialog") {
            auto type = file.read_name();
            auto name = file.read_name();
            
            DialogTopic* topic = DialogTopic::Make(name);
            
            topic->name = name;
            topic->prompt = file.read_name();
            topic->answer = file.read_name();
            
            if (type == "topic") {
                topic->type = DIALOG_TOPIC;
            } else if (type == "import-single") {
                topic->type = DIALOG_IMPORT_SINGLE;
            } else if (type == "import-multiple") {
                topic->type = DIALOG_IMPORT_MULTIPLE;
            } else {
                Log(Severity::WARNING, Kitchensink::System(), "Unknown dialog topic type '{}' in file:", type, path);
            }
            
        } else if (record == "action") {
            auto name = file.read_name();
            
            DialogTopic* topic = DialogTopic::Find(name);
            
            if (!topic) {
                Log(Severity::WARNING, Kitchensink::System(), "Can't find dialog topic '{}' in file:", name, path);
                continue;
            }
            
            topic->action.quest = file.read_name();
            topic->action.trigger = file.read_name();

        } else if (record == "condition") {
            auto name = file.read_name();
            
            DialogTopic* topic = DialogTopic::Find(name);
            
            if (!topic) {
                Log(Severity::WARNING, Kitchensink::System(), "Can't find dialog topic '{}' in file:", name, path);
                continue;
            }
            
            topic->condition.quest = file.read_name();
            topic->condition.variable = file.read_name();
            
        } else if (record == "next") {
            auto name = file.read_name();
            
            DialogTopic* topic = DialogTopic::Find(name);
            
            if (!topic) {
                Log(Severity::WARNING, Kitchensink::System(), "Can't find dialog topic '{}' in file:", name, path);
                continue;
            }
            
            topic->next_topics.push_back(file.read_name());
            
        } else {
            Log(Severity::WARNING, Kitchensink::System(), "Unknown dialog record '{}' in file:", record, path);
        }
        
        file.skip_linebreak();
    }
}


DialogController::DialogController(name_t initial_topic) {
    this->initial_topic = initial_topic;
    this->current_topic = initial_topic;
    
    // we could properly yeet in the greeting here
    this->answer = DialogTopic::Find(initial_topic)->answer;
}

name_t DialogController::GetAnswer() {
    return answer;
}

std::vector<name_t> DialogController::GetPrompts() {
    std::vector<name_t> prompts;
    for (auto topic : next_topics) {
        prompts.push_back(DialogTopic::Find(topic)->prompt);
    }
    if (current_topic == initial_topic) prompts.push_back("[exit]");
    return prompts;
}

bool DialogController::ShouldExit() {
    return should_exit;
}

void RecursivelyAppendNextTopic(std::vector<name_t>& topics, name_t appendable) {
    DialogTopic* topic = DialogTopic::Find(appendable);

    if (topic->condition.IsMet()) return;    

    switch (topic->type) {
        case DIALOG_TOPIC:
            topics.push_back(appendable);
            break;
        case DIALOG_IMPORT_SINGLE:
            for (auto topic : DialogTopic::Find(appendable)->next_topics) {
                DialogTopic* importable = DialogTopic::Find(appendable);
                if (importable->condition.IsMet()) {
                    RecursivelyAppendNextTopic(topics, topic);
                    return;
                }
            }
            break;
        case DIALOG_IMPORT_MULTIPLE:
            for (auto topic : DialogTopic::Find(appendable)->next_topics) {
                DialogTopic* importable = DialogTopic::Find(appendable);
                if (importable->condition.IsMet()) {
                    RecursivelyAppendNextTopic(topics, topic);
                }
            }
            break;
    }
}

void DialogController::MoveTo(size_t prompt_index) {
    if (prompt_index >= next_topics.size()) {
        should_exit = true;
        return;
    }
    
    name_t next_topic = next_topics[prompt_index];
    
    next_topics.clear();
    for (auto topic : DialogTopic::Find(next_topic)->next_topics) {
        RecursivelyAppendNextTopic(next_topics, topic);
    }
    
    if (!next_topics.size()) {
        for (auto topic : DialogTopic::Find(initial_topic)->next_topics) {
            RecursivelyAppendNextTopic(next_topics, topic);
        }
    }
    
    current_topic = next_topic;
    
    answer = DialogTopic::Find(current_topic)->answer;
    
    DialogTopic::Find(current_topic)->action.Perform();
}

}