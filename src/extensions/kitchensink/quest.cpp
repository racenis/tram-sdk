#include <extensions/kitchensink/quest.h>

#include <extensions/kitchensink/inventory.h>

#include <framework/entity.h>
#include <framework/script.h>
#include <framework/file.h>

#include <templates/pool.h>

#include <cstring>

using namespace tram;
using namespace tram::Ext::Kitchensink;
template<> Pool<Quest> PoolProxy<Quest>::pool("qyuespool", 10);

namespace tram::Ext::Kitchensink {

void QuestVariable::SetValue(Value value) {
    this->value1 = value;
    this->type = QUEST_VAR_VALUE;
}

void QuestVariable::SetIs(name_t quest1, name_t var1, name_t quest2, value_t var2) {
    this->quest1 = quest1;
    this->quest2 = quest2;
    this->value1 = var1;
    this->value2 = var2;
    this->type = QUEST_VAR_VALUE_IS;
}

void QuestVariable::SetIsNot(name_t quest1, name_t var1, name_t quest2, value_t var2) {
    SetIs(quest1, var1, quest2, var2);
    this->type = QUEST_VAR_VALUE_IS_NOT;
}

void QuestVariable::SetGreater(name_t quest1, name_t var1, name_t quest2, value_t var2) {
    SetIs(quest1, var1, quest2, var2);
    this->type = QUEST_VAR_VALUE_GREATER;
}

void QuestVariable::SetGreaterOrEqual(name_t quest1, name_t var1, name_t quest2, value_t var2) {
    SetIs(quest1, var1, quest2, var2);
    this->type = QUEST_VAR_VALUE_GREATER_OR_EQUAL;
}

void QuestVariable::SetLess(name_t quest1, name_t var1, name_t quest2, value_t var2) {
    SetIs(quest1, var1, quest2, var2);
    this->type = QUEST_VAR_VALUE_LESS;
}

void QuestVariable::SetLessOrEqual(name_t quest1, name_t var1, name_t quest2, value_t var2) {
    SetIs(quest1, var1, quest2, var2);
    this->type = QUEST_VAR_VALUE_LESS_OR_EQUAL;
}

void QuestVariable::SetNot(name_t quest, name_t var) {
    this->quest1 = quest;
    this->value1 = var;
    this->type = QUEST_VAR_CONDITION_NOT;
}

void QuestVariable::SetAnd(name_t quest1, name_t var1, name_t quest2, value_t var2) {
    SetIs(quest1, var1, quest2, var2);
    this->type = QUEST_VAR_CONDITION_AND;
}

void QuestVariable::SetOr(name_t quest1, name_t var1, name_t quest2, value_t var2) {
    SetIs(quest1, var1, quest2, var2);
    this->type = QUEST_VAR_CONDITION_OR;
}

void QuestVariable::SetScript(name_t script) {
    this->value1 = script;
    this->type = QUEST_VAR_CONDITION_SCRIPT;
}


void QuestVariable::SetObjective(name_t title, name_t description, int state) {
    this->value1 = title;
    this->value2 = description;
    
    this->state = state;
    this->type = QUEST_VAR_OBJECTIVE;
}

Value QuestVariable::Evaluate() {
    switch (type) {
        case QUEST_VAR_VALUE:
            return value1;
        case QUEST_VAR_VALUE_IS:
            return Quest::Find(quest1)->GetVariable(value1) == (quest2 ? Quest::Find(quest2)->GetVariable(value2) : value2);
        case QUEST_VAR_VALUE_IS_NOT:
            return Quest::Find(quest1)->GetVariable(value1) != (quest2 ? Quest::Find(quest2)->GetVariable(value2) : value2);
        case QUEST_VAR_VALUE_GREATER:
            return Quest::Find(quest1)->GetVariable(value1).GetFloat() > (quest2 ? Quest::Find(quest2)->GetVariable(value2) : value2).GetFloat();
        case QUEST_VAR_VALUE_GREATER_OR_EQUAL:
            return Quest::Find(quest1)->GetVariable(value1).GetFloat() >= (quest2 ? Quest::Find(quest2)->GetVariable(value2) : value2).GetFloat();
        case QUEST_VAR_VALUE_LESS:
            return Quest::Find(quest1)->GetVariable(value1).GetFloat() < (quest2 ? Quest::Find(quest2)->GetVariable(value2) : value2).GetFloat();
        case QUEST_VAR_VALUE_LESS_OR_EQUAL:
            return Quest::Find(quest1)->GetVariable(value1).GetFloat() <= (quest2 ? Quest::Find(quest2)->GetVariable(value2) : value2).GetFloat();
        case QUEST_VAR_CONDITION_NOT:
            return !Quest::Find(quest1)->GetVariable(value1);
        case QUEST_VAR_CONDITION_AND:
            return Quest::Find(quest1)->GetVariable(value1) && (quest2 ? Quest::Find(quest2)->GetVariable(value2) : value2);
        case QUEST_VAR_CONDITION_OR:
            return Quest::Find(quest1)->GetVariable(value1) || (quest2 ? Quest::Find(quest2)->GetVariable(value2) : value2);
        case QUEST_VAR_CONDITION_SCRIPT:
            return Script::CallFunction(value1, {});
        case QUEST_VAR_HAS_ITEM: {
            Entity* entity = Entity::Find((name_t)value1);
            if (!entity) return false;
            Inventory* inventory = Inventory::Find(entity);
            if (!inventory) return false;
            return inventory->GetItemCount(value2);
        }
        default:
            assert(false);
            abort();
    }
}
    
    




void QuestTrigger::SetValue(name_t variable, Value value) {
    this->value = value;
    this->variable = variable;
    this->type = QUEST_TGR_SET_VARIABLE;
}

void QuestTrigger::SetObjective(name_t name, name_t state) {
    this->variable = name;
    this->value = state;
    this->type = QUEST_TGR_SET_OBJECTIVE;
}

void QuestTrigger::SetIncrement(name_t name) {
    this->variable = name;
    this->type = QUEST_TGR_INCREMENT;
}
    
    
    
Value Quest::GetVariable(name_t name) {
    //std::cout << "serch" << std::endl;
    for (auto& variable : variables) {
        //std::cout << variable.name << "|" << name << std::endl;
        if (variable.name == name) {
            //std::cout << "Evaluating variable " << name << std::endl;
            return variable.Evaluate();
        }
    }
    //std::cout << "Returning default value for variable " << name << std::endl;
    // TODO: write a error message
    return false;
}

void Quest::SetVariable(name_t name, Value value) {
    for (auto& variable : variables) {
        if (variable.name == name) {
            variable.SetValue(value);
            std::cout << "Set value of variable " << name << std::endl; 
            return;
        }
    }
    
    std::cout << "Inserted new variable " << name << std::endl;
    
    QuestVariable new_variable;
    new_variable.SetValue(value);
    variables.push_back(new_variable);
}

void Quest::FireTrigger(name_t name) {
    std::cout << "finding tirgger" << std::endl;
    for (auto& trigger : triggers) {
        if (trigger.name != name) continue;
        
        if (trigger.condition) std::cout << trigger.name << " has condintuion!" << std::endl;
        if (!trigger.condition) std::cout <<  trigger.name << " dosent condintuion!" << std::endl;
        if (trigger.condition) std::cout << "condniton: " << trigger.condition << " = " << (bool)GetVariable(trigger.condition) << std::endl;
        
        // TODO: implement GetBool()
        if (trigger.condition && !GetVariable(trigger.condition)/*.GetBool()*/) continue;
        
        
        
        switch (trigger.type) {
            case QUEST_TGR_SET_VARIABLE:
                SetVariable(trigger.variable, trigger.value);
                break;
            case QUEST_TGR_SET_OBJECTIVE:
                std::cout << "Setting objective..." << std::endl;
                for (auto& obj : variables) {
                    if (obj.name != trigger.variable) continue;
                    obj.state = trigger.value;
                    std::cout << "Objective " << obj.name << " set to " << trigger.value.GetInt() << std::endl;
                }
                break;
            case QUEST_TGR_INCREMENT:
                SetVariable(trigger.variable, GetVariable(trigger.variable).GetInt() + 1);
                break;
            default:
                assert(false);
        }
    }
}

std::vector<Quest*> Quest::FindAll(bool unknown_also) {
    std::vector<Quest*> quests;
    for (auto& quest : PoolProxy<Quest>::GetPool()) {
        bool is_known = false;
        for (auto& objective : quest.variables) {
            if (objective.type != QUEST_VAR_OBJECTIVE) continue;
            
            if (objective.value1 != (value_t)(name_t)"unknown") is_known = true; 
        }
        
        if (is_known || unknown_also) quests.push_back(&quest);
    }
    
    return quests;
}

class QuestEntity : public Entity {
public:
    QuestEntity(name_t name) : Entity(name) {}
    void UpdateParameters() {}
    void SetParameters() {}
    void Load() {}
    void Unload() {}
    void Serialize() {}
    name_t GetType() { return "none"; }
    void MessageHandler(Message& msg) {
        name_t trigger = *(Value*)msg.data;
        std::cout << name << " triggered " << trigger << std::endl;
        Quest::Find(name)->FireTrigger(trigger);
    }
};

Quest* Quest::Find(name_t quest) {
    assert(quest);
    
    for (auto& q : PoolProxy<Quest>::GetPool()) {
        if (q.name == quest) return &q;
    }
    
    new QuestEntity(quest);

    return PoolProxy<Quest>::New(quest);
}




// TODO: yeet. instead create on constructor!!
void Quest::Init() {
    /*for (auto& q : PoolProxy<Quest>::GetPool()) {
        new QuestEntity(q.name);
    }*/
}

static std::pair<name_t, value_t> LoadVariableSecond(File& file) {
    name_t quest;
    value_t value;
    
    name_t type = file.read_name();
    
    if (type == "var") {
        quest = file.read_name();
        value = file.read_name();
    } else if (type == "bool") {
        //value = (bool)file.read_int32();
        value = file.read_name() == "true";
    } else if (type == "int") {
        value = file.read_int32();
    } else if (type == "float") {
        value = file.read_float32();
    } else if (type == "name") {
        value = file.read_name();
    } else {
        std::cout << "unknown variable comparison: " << type << std::endl;
        abort();
    }
    
    return {quest, value};
}

void Quest::LoadFromDisk(const char* filename) {
    char path [100] = "data/";
    strcat(path, filename);
    strcat(path, ".quest");

    File file (path, MODE_READ);

    if (!file.is_open()) {
        std::cout << "Can't open quest file '" << path << "'" << std::endl;
        abort();
    }

    name_t file_type = file.read_name();

    if (file_type != "QUESTv1") {
        std::cout << "Invalid quest file type " << path << std::endl;
        abort();
    }
    
    std::cout << "Loading: " << filename << std::endl;

    while (file.is_continue()) {
        if (auto record = file.read_name(); record != "quest") {
            std::cout << "unknown quest record: " << record << std::endl;
            abort();
        }
        
        name_t quest_name = file.read_name();
        
        Quest* quest = Quest::Find(quest_name);
        
        while (file.is_continue()) {
            name_t record_type = file.read_name();
            
            if (record_type == "variable") {
                name_t variable_name = file.read_name();
                name_t variable_type = file.read_name();
                
                QuestVariable variable;
                variable.name = variable_name;
                
                if (variable_type == "value") {
                    name_t type = file.read_name();

                    if (type == "bool") {
                        //variable.SetValue((bool)file.read_int32());
                        variable.SetValue(file.read_name() == "true");
                    } else if (type == "int") {
                        variable.SetValue(file.read_int32());
                    } else if (type == "float") {
                        variable.SetValue(file.read_float32());
                    } else if (type == "name") {
                        variable.SetValue(file.read_name());
                    } else {
                        std::cout << "unknown variable value type: " << type << std::endl;
                        abort();
                    }
                } else if (variable_type == "is") {
                    auto q1 = file.read_name();
                    auto v1 = file.read_name();
                    auto [q2, v2] = LoadVariableSecond(file);
                    variable.SetIs(q1, v1, q2, v2);
                } else if (variable_type == "is-not") {
                    auto q1 = file.read_name();
                    auto v1 = file.read_name();
                    auto [q2, v2] = LoadVariableSecond(file);
                    variable.SetIsNot(q1, v1, q2, v2);
                } else if (variable_type == "greater") {
                    auto q1 = file.read_name();
                    auto v1 = file.read_name();
                    auto [q2, v2] = LoadVariableSecond(file);
                    variable.SetGreater(q1, v1, q2, v2);
                } else if (variable_type == "greater-equal") {
                    auto q1 = file.read_name();
                    auto v1 = file.read_name();
                    auto [q2, v2] = LoadVariableSecond(file);
                    variable.SetGreaterOrEqual(q1, v1, q2, v2);
                } else if (variable_type == "less") {
                    auto q1 = file.read_name();
                    auto v1 = file.read_name();
                    auto [q2, v2] = LoadVariableSecond(file);
                    variable.SetLess(q1, v1, q2, v2);
                } else if (variable_type == "less-equal") {
                    auto q1 = file.read_name();
                    auto v1 = file.read_name();
                    auto [q2, v2] = LoadVariableSecond(file);
                    variable.SetLessOrEqual(q1, v1, q2, v2);
                } else if (variable_type == "not") {
                    auto q1 = file.read_name();
                    auto v1 = file.read_name();
                    variable.SetNot(q1, v1);
                } else if (variable_type == "and") {
                    auto q1 = file.read_name();
                    auto v1 = file.read_name();
                    auto [q2, v2] = LoadVariableSecond(file);
                    variable.SetAnd(q1, v1, q2, v2);
                }  else if (variable_type == "or") {
                    auto q1 = file.read_name();
                    auto v1 = file.read_name();
                    auto [q2, v2] = LoadVariableSecond(file);
                    variable.SetOr(q1, v1, q2, v2);
                } else if (variable_type == "script") {
                    auto q1 = file.read_name();
                    variable.SetScript(q1);
                }  else if (variable_type == "objective") {
                    auto title = file.read_name();
                    auto desc = file.read_name();
                    auto state = file.read_int32();
                    variable.SetObjective(title, desc, state);
                } else {
                    std::cout << "unknown variable type: " << variable_type << std::endl;
                    abort();
                }
                
                quest->variables.push_back(variable);
            } else if (record_type == "trigger") {
                name_t trigger_name = file.read_name();
                name_t condition_quest = file.read_name();
                name_t condition_name = file.read_name();
                name_t trigger_type = file.read_name();
                
                QuestTrigger trigger;
                trigger.name = trigger_name;
                trigger.condition = condition_name;
                // TODO: implement condition adding for quest also
                
                if (trigger_type == "set-value") {
                    auto [variable, value] = LoadVariableSecond(file);
                    trigger.SetValue(variable, value);
                } else if (trigger_type == "set-objective") {
                    auto objective = file.read_name();
                    auto state = file.read_name();
                    trigger.SetObjective(objective, state);
                } else if (trigger_type == "increment") {
                    auto variable = file.read_name();
                    trigger.SetIncrement(variable);
                } else {
                    std::cout << "unknown trigger type: " << trigger_type << std::endl;
                    abort();
                }
                
                quest->triggers.push_back(trigger);
            } else {
                std::cout << "unknown quest parameter: " << record_type << std::endl;
                abort();
            }
            
        }
    }
}

}