#include <extensions/kitchensink/dialog.h>
#include <extensions/kitchensink/quest.h>

#include <templates/pool.h>
#include <templates/hashmap.h>

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

}