#include <extensions/kitchensink/dialog.h>
#include <extensions/kitchensink/quest.h>

#include <templates/pool.h>
#include <templates/hashmap.h>

using namespace tram;
template <> Pool<Ext::Kitchensink::DialogTopic> PoolProxy<Ext::Kitchensink::DialogTopic>::pool("DialogTopic pool", 500);
static Hashmap<Ext::Kitchensink::DialogTopic*> item_class_list("DialogTopic list", 500);

namespace tram::Ext::Kitchensink {

bool DialogTopic::ConditionMet() {
    Value cond = Quest::Find(condition_quest)->GetVariable(condition_variable);
    assert(cond.GetType() == TYPE_BOOL);
    return (bool)cond;
}

void DialogTopic::PerformAction() {
    Quest::Find(action_quest)->FireTrigger(action_variable);
}

DialogTopic* DialogTopic::Make(name_t name) {
    DialogTopic* topic = item_class_list.Find(name);
    
    if (!topic) {
        topic = PoolProxy<DialogTopic>::New(name);
        item_class_list.Insert(UID(name), topic);
    }
    
    return topic;
}

DialogTopic* DialogTopic::Find(name_t name) {
    return item_class_list.Find(name);
}

}