#include <extensions/kitchensink/dialog.h>
#include <extensions/kitchensink/quest.h>

#include <templates/pool.h>
#include <templates/hashmap.h>

#include <framework/file.h>

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
    DialogTopic* topic = item_class_list.Find(name);
    
    if (!topic) {
        topic = PoolProxy<DialogTopic>::New();
        topic->name = name;
        item_class_list.Insert(UID(name), topic);
    }
    
    return topic;
}

DialogTopic* DialogTopic::Find(name_t name) {
    return item_class_list.Find(name);
}

void DialogTopic::LoadFromDisk(const char* filename) {
    char path [100] = "data/";
    strcat(path, filename);
    strcat(path, ".dialog");

    File file (path, MODE_READ);

    if (!file.is_open()) {
        std::cout << "Can't open dialog file '" << path << "'" << std::endl;
        abort();
    }

    name_t file_type = file.read_name();

    if (file_type != "DIALOGv1") {
        std::cout << "Invalid quest file type " << path << std::endl;
        abort();
    }
    
    std::cout << "Loading: " << filename << std::endl;

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
                std::cout << "unknown dialog topic type: " << type << std::endl;
                abort();
            }
            
        } else if (record == "action") {
            auto name = file.read_name();
            
            DialogTopic* topic = DialogTopic::Find(name);
            
            if (!topic) {
                std::cout << "can't find dialog topic: " << name << std::endl;
                abort();
            }
            
            topic->action.quest = file.read_name();
            topic->action.trigger = file.read_name();

        } else if (record == "condition") {
            auto name = file.read_name();
            
            DialogTopic* topic = DialogTopic::Find(name);
            
            if (!topic) {
                std::cout << "can't find dialog topic: " << name << std::endl;
                abort();
            }
            
            topic->condition.quest = file.read_name();
            topic->condition.variable = file.read_name();
            
        } else if (record == "next") {
            auto name = file.read_name();
            
            DialogTopic* topic = DialogTopic::Find(name);
            
            if (!topic) {
                std::cout << "can't find dialog topic: " << name << std::endl;
                abort();
            }
            
            topic->next_topics.push_back(file.read_name());
            
        }  else {
            std::cout << "unknown dialog record: " << record << std::endl;
            abort();
        }
    }
}

}