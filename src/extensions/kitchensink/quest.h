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
    QUEST_VAR_HAS_ITEM,
    
    
    QUEST_TGR_SET_VARIABLE,
    QUEST_TGR_SHOW_MESSAGE,
};
    
struct QuestVariable {
    name_t name;
    quest_t type;
    
    name_t target;
    
    Value value1;
    Value value2;
    
    name_t quest1;
    name_t quest2;
    
    void SetValue(Value);
    void SetIs(name_t, name_t, name_t, name_t);
    void SetIsNot(name_t, name_t, name_t, name_t);
    void SetGreater(name_t, name_t, name_t, name_t);
    void SetGreaterOrEqual(name_t, name_t, name_t, name_t);
    void SetGreaterLess(name_t, name_t, name_t, name_t);
    void SetGreaterLessOrEqual(name_t, name_t, name_t, name_t);
    void SetNot(name_t, name_t);
    void SetAnd(name_t, name_t, name_t, name_t);
    void SetOr(name_t, name_t, name_t, name_t);
    void SetScript(name_t);
    
    Value Evaluate();
};

struct QuestTrigger {
    name_t name;
    quest_t type;
    
    Value value;
    
    void SetValue(Value);
    
    void Fire();
};

struct Quest {
    name_t name;

    std::vector<QuestVariable> variables;
    std::vector<QuestTrigger> triggers;
    
    Value GetVariable(name_t);
    void SetVariable(name_t, Value);
    
    void FireTrigger(name_t);
    
    static Quest* Find(name_t quest);
};
    
}

#endif // TRAM_SDK_EXTENSIONS_KITCHENSINK_QUEST_H