// TRAMWAY DRIFT AND DUNGEON EXPLORATION SIMULATOR 2022
// All rights reserved.

#ifndef TRAM_SDK_EXTENSIONS_KITCHENSINK_QUEST_H
#define TRAM_SDK_EXTENSIONS_KITCHENSINK_QUEST_H

#include <framework/value.h>

#include <vector>

namespace tram::Ext::Kitchensink {

enum quest_t : int {
    QUEST_VAR_VALUE,
    QUEST_VAR_VALUE_IS,
    QUEST_VAR_VALUE_IS_NOT,
    QUEST_VAR_VALUE_GREATER,
    QUEST_VAR_VALUE_GREATER_OR_EQUAL,
    QUEST_VAR_VALUE_LESS,
    QUEST_VAR_VALUE_LESS_OR_EQUAL,
    QUEST_VAR_CONDITION_NOT,
    QUEST_VAR_CONDITION_AND,
    QUEST_VAR_CONDITION_OR,
    QUEST_VAR_CONDITION_SCRIPT,
    QUEST_VAR_OBJECTIVE,
    QUEST_VAR_HAS_ITEM,
    
    
    QUEST_TGR_SET_VARIABLE,
    QUEST_TGR_SET_OBJECTIVE,
    QUEST_TGR_INCREMENT,
    QUEST_TGR_SHOW_MESSAGE,
};
    
struct QuestVariable {
    name_t name;
    quest_t type;
    
    name_t target;
    
    Value value1;
    Value value2;
    
    int state;
    
    name_t quest1;
    name_t quest2;
    
    void SetValue(Value);
    void SetIs(name_t, name_t, name_t, value_t);
    void SetIsNot(name_t, name_t, name_t, value_t);
    void SetGreater(name_t, name_t, name_t, value_t);
    void SetGreaterOrEqual(name_t, name_t, name_t, value_t);
    void SetLess(name_t, name_t, name_t, value_t);
    void SetLessOrEqual(name_t, name_t, name_t, value_t);
    void SetNot(name_t, name_t);
    void SetAnd(name_t, name_t, name_t, value_t);
    void SetOr(name_t, name_t, name_t, value_t);
    void SetScript(name_t);
    
    void SetObjective(name_t, name_t, int);
    
    Value Evaluate();
};

struct QuestTrigger {
    name_t name;
    quest_t type;
    
    name_t condition; // condition, if any, for firing
    
    name_t variable; // name for variable which will be set
    Value value; // variable name for which will be set
    
    void SetValue(name_t variable, Value);
    void SetObjective(name_t name, int state);
    void SetIncrement(name_t variable);
};

struct Quest {
    Quest(name_t name) : name(name) {}
    
    name_t name;

    std::vector<QuestVariable> variables;
    std::vector<QuestTrigger> triggers;
    
    // TODO: add trigger script
    // if can't find in trigger list, then call script
    
    Value GetVariable(name_t);
    void SetVariable(name_t, Value);
    
    void FireTrigger(name_t);
    
    static void Init();
    
    static void LoadFromDisk(const char* filename);
    static Quest* Find(name_t quest);
};
    
}

#endif // TRAM_SDK_EXTENSIONS_KITCHENSINK_QUEST_H