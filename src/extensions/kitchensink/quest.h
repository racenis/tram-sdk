// Tramway Drifting and Dungeon Exploration Simulator SDK Runtime

#ifndef TRAM_SDK_EXTENSIONS_KITCHENSINK_QUEST_H
#define TRAM_SDK_EXTENSIONS_KITCHENSINK_QUEST_H

#include <framework/value.h>

#include <vector>

namespace tram::Ext::Kitchensink {

enum quest_t : int {
    QUEST_VAR_INVALID,
    
    QUEST_VAR_VALUE,
    QUEST_VAR_OBJECTIVE,
    
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
    
    
    QUEST_VAR_ENTITY_ITEM_COUNT,
    
    
    QUEST_TGR_SET_VARIABLE,
    QUEST_TGR_SET_OBJECTIVE,
    QUEST_TGR_INCREMENT,
    QUEST_TGR_SHOW_MESSAGE,
};

struct QuestVariable {
    name_t name;
    quest_t type;

    union {
        struct {
            Value value;
        } value;
        
        struct {
            name_t value;
            name_t title;
            name_t subtitle;
        } objective;
        
        struct {
            name_t name;
        } script;
        
        struct {
            name_t quest1;
            name_t variable1;
            
            // if quest2 set, then variable2 is name of the quest's variable
            // if quest is not set, then variable2 is the Value for comparison
            name_t quest2;
            value_t variable2;
            
        } comparison;
        
        struct {
            name_t entity;
            name_t item;
            int count;
        } item;
    };
    
    static QuestVariable Value(name_t, value_t);
    static QuestVariable Objective(name_t, name_t, name_t, name_t);
    
    static QuestVariable Is(name_t, name_t, name_t, name_t, value_t);
    static QuestVariable IsNot(name_t, name_t, name_t, name_t, value_t);
    static QuestVariable Greater(name_t, name_t, name_t, name_t, value_t);
    static QuestVariable GreaterOrEqual(name_t, name_t, name_t, name_t, value_t);
    static QuestVariable Less(name_t, name_t, name_t, name_t, value_t);
    static QuestVariable LessOrEqual(name_t, name_t, name_t, name_t, value_t);
    
    static QuestVariable And(name_t, name_t, name_t, name_t, value_t);
    static QuestVariable Or(name_t, name_t, name_t, name_t, value_t);
    
    static QuestVariable Not(name_t, name_t, name_t);
    
    static QuestVariable Script(name_t, name_t);
    
    static QuestVariable ItemCount(name_t, name_t, name_t);
    
    QuestVariable() { type = QUEST_VAR_INVALID; }
    ~QuestVariable() {}
    
    QuestVariable(const QuestVariable&);
    QuestVariable& operator=(const QuestVariable&);
    //QuestVariable& operator=(QuestVariable& a, const QuestVariable& b);
    
    value_t Evaluate();
};

struct QuestTrigger {
    name_t name;
    quest_t type;
    
    name_t condition; // condition, if any, for firing
    
    name_t variable; // name for variable which will be set
    value_t value; // variable name for which will be set
    
    void SetValue(name_t variable, Value);
    void SetObjective(name_t name, name_t state);
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
    
    
    static std::vector<Quest*> FindAll(bool unknown_also);
    
    static void LoadFromDisk(const char* filename);
    static Quest* Find(name_t quest);
};
    
}

#endif // TRAM_SDK_EXTENSIONS_KITCHENSINK_QUEST_H