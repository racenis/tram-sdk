#include <extensions/kitchensink/quest.h>

#include <framework/script.h>

#include <templates/pool.h>

using namespace tram;
using namespace tram::Ext::Kitchensink;
template<> Pool<Quest> PoolProxy<Quest>::pool("qyuespool", 10);

namespace tram::Ext::Kitchensink {

void QuestVariable::SetValue(Value value) {
    this->value1 = value;
    this->type = QUEST_VAR_VALUE;
}

void QuestVariable::SetIs(name_t quest1, name_t var1, name_t quest2, name_t var2) {
    this->quest1 = quest1;
    this->quest2 = quest2;
    this->value1 = var1;
    this->value2 = var2;
    this->type = QUEST_VAR_VALUE_IS;
}

void QuestVariable::SetIsNot(name_t quest1, name_t var1, name_t quest2, name_t var2) {
    SetIs(quest1, var1, quest2, var2);
    this->type = QUEST_VAR_VALUE_IS_NOT;
}

void QuestVariable::SetGreater(name_t quest1, name_t var1, name_t quest2, name_t var2) {
    SetIs(quest1, var1, quest2, var2);
    this->type = QUEST_VAR_VALUE_GREATER;
}

void QuestVariable::SetGreaterOrEqual(name_t quest1, name_t var1, name_t quest2, name_t var2) {
    SetIs(quest1, var1, quest2, var2);
    this->type = QUEST_VAR_VALUE_GREATER_OR_EQUAL;
}

void QuestVariable::SetGreaterLess(name_t quest1, name_t var1, name_t quest2, name_t var2) {
    SetIs(quest1, var1, quest2, var2);
    this->type = QUEST_VAR_VALUE_LESS;
}

void QuestVariable::SetGreaterLessOrEqual(name_t quest1, name_t var1, name_t quest2, name_t var2) {
    SetIs(quest1, var1, quest2, var2);
    this->type = QUEST_VAR_VALUE_LESS_OR_EQUAL;
}

void QuestVariable::SetNot(name_t quest, name_t var) {
    this->quest1 = quest;
    this->value1 = var;
    this->type = QUEST_VAR_CONDITION_NOT;
}

void QuestVariable::SetAnd(name_t quest1, name_t var1, name_t quest2, name_t var2) {
    SetIs(quest1, var1, quest2, var2);
    this->type = QUEST_VAR_CONDITION_AND;
}

void QuestVariable::SetOr(name_t quest1, name_t var1, name_t quest2, name_t var2) {
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
            return Quest::Find(quest1)->GetVariable(value1) == Quest::Find(quest2)->GetVariable(value2);
        case QUEST_VAR_VALUE_IS_NOT:
            return Quest::Find(quest1)->GetVariable(value1) != Quest::Find(quest2)->GetVariable(value2);
        case QUEST_VAR_VALUE_GREATER:
            return Quest::Find(quest1)->GetVariable(value1).GetFloat() > Quest::Find(quest2)->GetVariable(value2).GetFloat();
        case QUEST_VAR_VALUE_GREATER_OR_EQUAL:
            return Quest::Find(quest1)->GetVariable(value1).GetFloat() >= Quest::Find(quest2)->GetVariable(value2).GetFloat();
        case QUEST_VAR_VALUE_LESS:
            return Quest::Find(quest1)->GetVariable(value1).GetFloat() < Quest::Find(quest2)->GetVariable(value2).GetFloat();
        case QUEST_VAR_VALUE_LESS_OR_EQUAL:
            return Quest::Find(quest1)->GetVariable(value1).GetFloat() <= Quest::Find(quest2)->GetVariable(value2).GetFloat();
        case QUEST_VAR_CONDITION_NOT:
            return !Quest::Find(quest1)->GetVariable(value1);
        case QUEST_VAR_CONDITION_AND:
            return Quest::Find(quest1)->GetVariable(value1) && Quest::Find(quest2)->GetVariable(value2);
        case QUEST_VAR_CONDITION_OR:
            return Quest::Find(quest1)->GetVariable(value1) || Quest::Find(quest2)->GetVariable(value2);
        case QUEST_VAR_CONDITION_SCRIPT:
            return Script::CallFunction(value1, {});
        case QUEST_VAR_HAS_ITEM:
            
            // TODO: implement
        default:
            assert(false);
            abort();
    }
}
    
    
    
    
    
Value Quest::GetVariable(name_t name) {
    for (auto& variable : variables) {
        if (variable.name == name) return variable.Evaluate();
    }
    return false;
}

void Quest::SetVariable(name_t name, Value value) {
    for (auto& variable : variables) {
        if (variable.name == name) {
            variable.SetValue(value);
            return;
        }
    }
    
    QuestVariable new_variable;
    new_variable.SetValue(value);
    variables.push_back(new_variable);
}

void Quest::FireTrigger(name_t name) {
    for (auto& trigger : triggers) {
        if (trigger.name != name) continue;
        
        switch (trigger.type) {
            case QUEST_TGR_SET_VARIABLE:
                // ...
                break;
            default:
                assert(false);
        }
    }
}
    
Quest* Quest::Find(name_t quest) {
    for (auto& q : PoolProxy<Quest>::GetPool()) {
        if (q.name == quest) return &q;
    }

    return PoolProxy<Quest>::New();
}

}