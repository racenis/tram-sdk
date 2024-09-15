// TRAMWAY DRIFT AND DUNGEON EXPLORATION SIMULATOR 2022
// All rights reserved.

#ifndef TRAM_SDK_EXTENSIONS_KITCHENSINK_AI_H
#define TRAM_SDK_EXTENSIONS_KITCHENSINK_AI_H

#include <vector>
#include <framework/core.h>
#include <framework/uid.h>
#include <framework/math.h>
#include <framework/value.h>

namespace tram {
    class ControllerComponent;
}

namespace tram::Ext::Kitchensink {
    class AnimationStateComponent;
    /*
    
    if normal:

    if time is morning:
        return go_eat()
    if time is day:
        return go_stand(at_workplace)
    if time is evening:
        return go_eat()
    if time is night:
        return go_sleep(home)

    if combat:

    if health > 10:
        return attack
    if true:
        return retreat

    inside_params -> 
        attacker
        attacker_faction

    if normal and attack and victim_faction is self_faction:
        combat_target = 
        attack

    AIAgent:
      list<pair<AICondition, AIPackage>> package_queue;
      name class;
      package_script
      list<Value> variables
    AIClass:
      name parent_class;
      package_script;
      default_variables;

    AIPackage:
      state = running/finished
      forced = true/false

    SetAIPackage(AI_ATTACK, [("target", big_chungus)])

    AI_ATTACK
    AI_SLEEP
    AI_EAT
      

AI_ATTACK ->

 */

class AIAgent;
class AIPackage;
typedef AIPackage* (*ai_package_constr)(valuearray_t);

void InitAI();
void UpdateAI();

class AIPackage {
public:
    virtual bool Process() = 0;
    
    virtual ~AIPackage() = default;

    static void Register(name_t name, ai_package_constr constr);    
    static AIPackage* Make(name_t name, valuearray_t params);
protected:
    AIAgent* parent = nullptr;
};

struct AIVariable {
    Value value;
};

class AIAgent {
public:
    std::vector<AIPackage*> package_stack;
    std::vector<AIPackage*> override_stack;
    
    name_t script;

    std::vector<AIVariable> variables;
    
    void Push(AIPackage*);
    void Clear();
    
    void PushOverride(AIPackage*);
    void ClearOverride();

    void Update();

    void SetControllerComponent(ControllerComponent* c) { controllercomponent = c; }
    void SetAnimationStateComponent(AnimationStateComponent* c) { animationstate = c; }
    ControllerComponent* GetControllerComponent() { return controllercomponent; }
    AnimationStateComponent* GetAnimationStateComponent() { return animationstate; }

    static AIAgent* Find(id_t id);
private:
    ControllerComponent* controllercomponent;
    AnimationStateComponent* animationstate;
};

}

#endif // TRAM_SDK_EXTENSIONS_KITCHENSINK_AI_H