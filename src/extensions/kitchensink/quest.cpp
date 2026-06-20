// Tramway Drifting and Dungeon Exploration Simulator SDK Runtime

#include <extensions/kitchensink/quest.h>

#include <extensions/kitchensink/inventory.h>
#include <extensions/kitchensink/Kitchensink.h>

#include <framework/entity.h>
#include <framework/script.h>
#include <framework/file.h>

#include <templates/pool.h>

#include <cstring>

using namespace tram;
using namespace tram::Ext::Kitchensink;
template<> Pool<Quest> PoolProxy<Quest>::pool("qyuespool", 10);

namespace tram::Ext::Kitchensink {

// added (void*)s to make the compiler stop complaining.
// it's proabably safe though.
// might have to look it up, but I think it has something to do with glm using
// tons of unions internally and the standards people at the C++ store don't
// really like it, but on most compilers it should be fine
QuestVariable::QuestVariable(const QuestVariable& other) {
    memcpy((void*)this, (void*)&other, sizeof(QuestVariable));
}

QuestVariable& QuestVariable::operator=(const QuestVariable& other) {
    memcpy((void*)this, (void*)&other, sizeof(QuestVariable));
    return *this;
}
    
QuestVariable QuestVariable::ValueVariable(name_t name, value_t value) {
    QuestVariable var;
    
    var.type = QUEST_VAR_VALUE;
    var.name = name;
    
    var.value.value = value;
    
    return var;
}

QuestVariable QuestVariable::Is(name_t name, name_t quest1, name_t var1, name_t quest2, value_t var2) {
    QuestVariable var;
    
    var.type = QUEST_VAR_VALUE_IS;
    var.name = name;
    
    var.comparison.quest1 = quest1;
    var.comparison.variable1 = var1;
    var.comparison.quest2 = quest2;
    var.comparison.variable2 = var2;
    
    return var;
}

QuestVariable QuestVariable::IsNot(name_t name, name_t quest1, name_t var1, name_t quest2, value_t var2) {
    QuestVariable var;
    
    var.type = QUEST_VAR_VALUE_IS_NOT;
    var.name = name;
    
    var.comparison.quest1 = quest1;
    var.comparison.variable1 = var1;
    var.comparison.quest2 = quest2;
    var.comparison.variable2 = var2;
    
    return var;
}

QuestVariable QuestVariable::Greater(name_t name, name_t quest1, name_t var1, name_t quest2, value_t var2) {
    QuestVariable var;
    
    var.type = QUEST_VAR_VALUE_GREATER;
    var.name = name;
    
    var.comparison.quest1 = quest1;
    var.comparison.variable1 = var1;
    var.comparison.quest2 = quest2;
    var.comparison.variable2 = var2;
    
    return var;
}

QuestVariable QuestVariable::GreaterOrEqual(name_t name, name_t quest1, name_t var1, name_t quest2, value_t var2) {
    QuestVariable var;
    
    var.type = QUEST_VAR_VALUE_GREATER_OR_EQUAL;
    var.name = name;
    
    var.comparison.quest1 = quest1;
    var.comparison.variable1 = var1;
    var.comparison.quest2 = quest2;
    var.comparison.variable2 = var2;
    
    return var;
}

QuestVariable QuestVariable::Less(name_t name, name_t quest1, name_t var1, name_t quest2, value_t var2) {
    QuestVariable var;
    
    var.type = QUEST_VAR_VALUE_LESS;
    var.name = name;
    
    var.comparison.quest1 = quest1;
    var.comparison.variable1 = var1;
    var.comparison.quest2 = quest2;
    var.comparison.variable2 = var2;
    
    return var;
}

QuestVariable QuestVariable::LessOrEqual(name_t name, name_t quest1, name_t var1, name_t quest2, value_t var2) {
    QuestVariable var;
    
    var.type = QUEST_VAR_VALUE_LESS_OR_EQUAL;
    var.name = name;
    
    var.comparison.quest1 = quest1;
    var.comparison.variable1 = var1;
    var.comparison.quest2 = quest2;
    var.comparison.variable2 = var2;
    
    return var;
}

QuestVariable QuestVariable::Not(name_t name, name_t quest1, name_t var1) {
    QuestVariable var;
    
    var.type = QUEST_VAR_CONDITION_NOT;
    var.name = name;
    
    var.comparison.quest1 = quest1;
    var.comparison.variable1 = var1;
    var.comparison.quest2 = quest1;
    var.comparison.variable2 = var1;
    
    return var;
}

QuestVariable QuestVariable::And(name_t name, name_t quest1, name_t var1, name_t quest2, value_t var2) {
    QuestVariable var;
    
    var.type = QUEST_VAR_CONDITION_AND;
    var.name = name;
    
    var.comparison.quest1 = quest1;
    var.comparison.variable1 = var1;
    var.comparison.quest2 = quest2;
    var.comparison.variable2 = var2;
    
    return var;
}

QuestVariable QuestVariable::Or(name_t name, name_t quest1, name_t var1, name_t quest2, value_t var2) {
    QuestVariable var;
    
    var.type = QUEST_VAR_CONDITION_OR;
    var.name = name;
    
    var.comparison.quest1 = quest1;
    var.comparison.variable1 = var1;
    var.comparison.quest2 = quest2;
    var.comparison.variable2 = var2;
    
    return var;
}

QuestVariable QuestVariable::Script(name_t name, name_t script) {
    QuestVariable var;
    
    var.type = QUEST_VAR_CONDITION_SCRIPT;
    var.name = name;
    
    var.script.name = script;
    
    return var;
}

QuestVariable QuestVariable::Objective(name_t name, name_t value, name_t title, name_t description) {
    QuestVariable var;
    
    var.type = QUEST_VAR_OBJECTIVE;
    var.name = name;
    
    var.objective.value = value;
    var.objective.title = title;
    var.objective.subtitle = description;
    
    return var;
}

QuestVariable QuestVariable::ItemCount(name_t name, name_t entity, name_t item) {
    QuestVariable var;
    
    var.type = QUEST_VAR_ENTITY_ITEM_COUNT;
    var.name = name;
    
    var.item.entity = entity;
    var.item.item = item;
    
    return var;
}

static value_t GetFirst(const QuestVariable* var) {
    return Quest::Find(var->comparison.quest1)->GetVariable(var->comparison.variable1);
}

static value_t GetSecond(const QuestVariable* var) {
    if (var->comparison.quest2) {
        return Quest::Find(var->comparison.quest2)->GetVariable(var->comparison.variable2);
    } else {
        return var->comparison.variable2;
    }
}

value_t QuestVariable::Evaluate() {
    switch (type) {
        case QUEST_VAR_INVALID:
            Log(Severity::WARNING, Kitchensink::System(), "Attempting to evaluate invalid quest variable '{}'", name);
            return false;
        case QUEST_VAR_VALUE:
            return value.value;
        case QUEST_VAR_OBJECTIVE:
            return value.value;
        case QUEST_VAR_VALUE_IS:
            return GetFirst(this) == GetSecond(this);
        case QUEST_VAR_VALUE_IS_NOT:
            return GetFirst(this) != GetSecond(this);
        case QUEST_VAR_VALUE_GREATER:
            return GetFirst(this).GetFloat() > GetSecond(this).GetFloat();
        case QUEST_VAR_VALUE_GREATER_OR_EQUAL:
            return GetFirst(this).GetFloat() >= GetSecond(this).GetFloat();
        case QUEST_VAR_VALUE_LESS:
            return GetFirst(this).GetFloat() < GetSecond(this).GetFloat();
        case QUEST_VAR_VALUE_LESS_OR_EQUAL:
            return GetFirst(this).GetFloat() <= GetSecond(this).GetFloat();
        case QUEST_VAR_CONDITION_NOT:
            return !GetFirst(this);
        case QUEST_VAR_CONDITION_AND:
            return GetFirst(this) && GetSecond(this);
        case QUEST_VAR_CONDITION_OR:
            return GetFirst(this) || GetSecond(this);
        case QUEST_VAR_CONDITION_SCRIPT:
            return Script::CallFunction(script.name, {});
        case QUEST_VAR_ENTITY_ITEM_COUNT: {
            Entity* entity = Entity::Find(item.entity);
            if (!entity) return false;
            Inventory* inventory = Inventory::Find(entity);
            if (!inventory) return false;
            return inventory->GetItemCount(item.item);
        }
        default:
            Log(Severity::WARNING, Kitchensink::System(), "Can't evaluate quest variable type {}!", type);
            return false;
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
    for (auto& variable : variables) {
        if (variable.name == name) {
            return variable.Evaluate();
        }
    }
    // TODO: write a error message
    return false;
}

void Quest::SetVariable(name_t name, value_t value) {
    for (auto& variable : variables) {
        if (variable.name != name) continue;
        
        switch (variable.type) {
            case QUEST_VAR_VALUE: variable.value.value = value;             break;
            case QUEST_VAR_OBJECTIVE: variable.objective.value = value;     break;
            default: variable = QuestVariable::ValueVariable(name, value);
        }
        
        Log(Severity::INFO, Kitchensink::System(), "Set value of quest variable '{}' in quest {}", name, this->name);
        return;
        
    }
    
    Log(Severity::INFO, Kitchensink::System(), "Inserter new quest variable '{}' in quest {}", name, this->name);
    
    QuestVariable new_variable = QuestVariable::ValueVariable(name, value);
    variables.push_back(new_variable);
}

void Quest::FireTrigger(name_t name) {
    for (auto& trigger : triggers) {
        if (trigger.name != name) continue;
        
        // TODO: implement GetBool()
        if (trigger.condition && !GetVariable(trigger.condition)/*.GetBool()*/) continue;
        
        switch (trigger.type) {
            case QUEST_TGR_SET_VARIABLE:
                SetVariable(trigger.variable, trigger.value);
                break;
            case QUEST_TGR_SET_OBJECTIVE:
                for (auto& obj : variables) {
                    if (obj.name != trigger.variable) continue;
                    obj.objective.value = trigger.value;
                    Log(Severity::INFO, Kitchensink::System(), "Objective {} set to '{}' in quest {}", obj.name, trigger.value.GetInt(), this->name);
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
            
            if (objective.objective.value != "unknown") is_known = true; 
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
    void Serialize(ValueArray&) {}
    name_t GetType() { return "none"; }
    void MessageHandler(Message& msg) {
        name_t trigger = *(Value*)msg.data;
        Log(Severity::INFO, Kitchensink::System(), "Received message for trigger '{}' in quest {}", trigger, name);
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

void Quest::Init() {

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
        Log(Severity::WARNING, Kitchensink::System(), "Unknown variable comparison '{}' in file:", type, file.get_path());
        value = false;
    }
    
    return {quest, value};
}

void Quest::LoadFromDisk(const char* filename) {
    char path [PATH_LIMIT] = "data/";
    strcat(path, filename);
    strcat(path, ".quest");

    File file (path, File::READ | File::PAUSE_LINE);

    if (!file.is_open()) {
        Log(Severity::NOTE, Kitchensink::System(), "Can't open quest file: {}", path);
        return;
    }

    name_t file_type = file.read_name(); file.skip_linebreak();

    if (file_type != "QUESTv1") {
        Log(Severity::WARNING, Kitchensink::System(), "Invalid file type '{}' in quest file: {}", file_type, path);
        return;
    }
    
    Log(Severity::INFO, Kitchensink::System(), "Loading quest: {}", path);

    while (file.is_continue()) {
        if (auto record = file.read_name(); record != "quest") {
            Log(Severity::WARNING, Kitchensink::System(), "Unknown quest record '{}' in file:", record, path);
            file.skip_linebreak();
            continue;
        }
        
        name_t quest_name = file.read_name(); file.skip_linebreak();
        
        Quest* quest = Quest::Find(quest_name);
        
        while (file.is_continue()) {
            name_t record_type = file.read_name();
            
            if (record_type == "variable") {
                name_t variable_name = file.read_name();
                name_t variable_type = file.read_name();
                
                QuestVariable variable;
                
                if (variable_type == "value") {
                    name_t type = file.read_name();

                    if (type == "bool") {
                        variable = QuestVariable::ValueVariable(variable_name, file.read_name() == "true");
                    } else if (type == "int") {
                        variable = QuestVariable::ValueVariable(variable_name, file.read_int32());
                    } else if (type == "float") {
                        variable = QuestVariable::ValueVariable(variable_name, file.read_float32());
                    } else if (type == "name") {
                        variable = QuestVariable::ValueVariable(variable_name, file.read_name());
                    } else {
                        Log(Severity::WARNING, Kitchensink::System(), "Unknown variable value type '{}' in file:", type, path);
                        file.skip_linebreak(); continue;
                    }
                } else if (variable_type == "is") {
                    auto q1 = file.read_name();
                    auto v1 = file.read_name();
                    auto [q2, v2] = LoadVariableSecond(file);
                    variable = QuestVariable::Is(variable_name, q1, v1, q2, v2);
                } else if (variable_type == "is-not") {
                    auto q1 = file.read_name();
                    auto v1 = file.read_name();
                    auto [q2, v2] = LoadVariableSecond(file);
                    variable = QuestVariable::IsNot(variable_name, q1, v1, q2, v2);
                } else if (variable_type == "greater") {
                    auto q1 = file.read_name();
                    auto v1 = file.read_name();
                    auto [q2, v2] = LoadVariableSecond(file);
                    variable = QuestVariable::Greater(variable_name, q1, v1, q2, v2);
                } else if (variable_type == "greater-equal") {
                    auto q1 = file.read_name();
                    auto v1 = file.read_name();
                    auto [q2, v2] = LoadVariableSecond(file);
                    variable = QuestVariable::GreaterOrEqual(variable_name, q1, v1, q2, v2);
                } else if (variable_type == "less") {
                    auto q1 = file.read_name();
                    auto v1 = file.read_name();
                    auto [q2, v2] = LoadVariableSecond(file);
                    variable = QuestVariable::Less(variable_name, q1, v1, q2, v2);
                } else if (variable_type == "less-equal") {
                    auto q1 = file.read_name();
                    auto v1 = file.read_name();
                    auto [q2, v2] = LoadVariableSecond(file);
                    variable = QuestVariable::LessOrEqual(variable_name, q1, v1, q2, v2);
                } else if (variable_type == "not") {
                    auto q1 = file.read_name();
                    auto v1 = file.read_name();
                    variable = QuestVariable::Not(variable_name, q1, v1);
                } else if (variable_type == "and") {
                    auto q1 = file.read_name();
                    auto v1 = file.read_name();
                    auto [q2, v2] = LoadVariableSecond(file);
                    variable = QuestVariable::And(variable_name, q1, v1, q2, v2);
                }  else if (variable_type == "or") {
                    auto q1 = file.read_name();
                    auto v1 = file.read_name();
                    auto [q2, v2] = LoadVariableSecond(file);
                    variable = QuestVariable::Or(variable_name, q1, v1, q2, v2);
                } else if (variable_type == "script") {
                    auto q1 = file.read_name();
                    variable = QuestVariable::Script(variable_name, q1);
                }  else if (variable_type == "objective") {
                    auto title = file.read_name();
                    auto desc = file.read_name();
                    auto state = file.read_name();
                    variable = QuestVariable::Objective(variable_name, state, title, desc);
                } else {
                    Log(Severity::WARNING, Kitchensink::System(), "Unknown variable type '{}' in file:", variable_type, path);
                    file.skip_linebreak(); continue;
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
                    Log(Severity::WARNING, Kitchensink::System(), "Unknown trigger type '{}' in file:", trigger_type, path);
                    file.skip_linebreak(); continue;
                }
                
                quest->triggers.push_back(trigger);
            } else {
                Log(Severity::WARNING, Kitchensink::System(), "Unknown quest parameter '{}' in file:", record_type, path);
            }
            
            file.skip_linebreak();
        }
    }
}

}