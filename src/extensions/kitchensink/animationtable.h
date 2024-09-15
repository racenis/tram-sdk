// TRAMWAY DRIFT AND DUNGEON EXPLORATION SIMULATOR 2022
// All rights reserved.

#ifndef TRAM_SDK_EXTENSIONS_KITCHENSINK_SOUNDTABLE_H
#define TRAM_SDK_EXTENSIONS_KITCHENSINK_SOUNDTABLE_H

#include <vector>
#include <framework/uid.h>
#include <framework/math.h>
#include <framework/entitycomponent.h>

namespace tram {
    class AnimationComponent;
}

namespace tram::Ext::Kitchensink {

struct AnimStateSpace {
    name_t name;
    name_t parent;
};

struct AnimState {
    name_t name;
    
    name_t animation;
    uint32_t repeats;
    float weight;
    float speed;
    bool interpolate;
    bool pause_last;

    name_t state_space;

    name_t next_state;
};

struct AnimStateTransitionFrom {
    name_t from_state;
    name_t into_state;

    float fade_time;
};

struct AnimStateTransitionInto {
    name_t from_state;
    name_t into_state;
    
    float fade_time;
};

struct AnimStateTransitionDefault {
    name_t state;
    
    float fade_in_time;
    float fade_out_time;
};

class AnimationStateComponent;

class AnimationTable {
public:
    AnimationTable(name_t name) : name(name) {}
    
    void SwitchState(name_t state, AnimationStateComponent* state_component);
    
    void AddStateSpace(name_t name, name_t parent);
    void AddState(name_t state, name_t space);
    
    void SetStateAnimation(name_t state, name_t animation_name, uint32_t repeats, float weight, float speed, bool pause_last);
    void SetStateNextState(name_t state, name_t next_state);

    void AddTransition(AnimStateTransitionFrom);
    void AddTransition(AnimStateTransitionInto);
    void AddTransition(AnimStateTransitionDefault);

    static AnimationTable* Find(name_t name);
private:
    AnimationComponent* component = nullptr;

    name_t name;

    std::vector<AnimStateSpace> state_spaces;
    std::vector<AnimState> states;
    
    std::vector<AnimStateTransitionFrom> transition_from;
    std::vector<AnimStateTransitionInto> transition_into;
    std::vector<AnimStateTransitionDefault> transition_default;
};

class AnimationStateComponent : public EntityComponent {
public:
    void EventHandler(Event &event) {}
    void Start();
    
    void SwitchState(name_t state) { table->SwitchState(state, this); }
    
    void SetAnimationTable(name_t name) { this->table = AnimationTable::Find(name); }
    void SetAnimationComponent(AnimationComponent* component) { this->component = component; }
    AnimationComponent* GetAnimationComponent() { return this->component; }
protected:
    AnimationTable* table = nullptr;
    AnimationComponent* component = nullptr;
    std::vector<name_t> states;
    friend class AnimationTable;
};

}

#endif // TRAM_SDK_EXTENSIONS_KITCHENSINK_SOUNDTABLE_H