// TRAMWAY DRIFT AND DUNGEON EXPLORATION SIMULATOR 2022
// All rights reserved.

#include <extensions/kitchensink/animationtable.h>

#include <render/render.h>
#include <entities/sound.h>

#include <components/animation.h>

#include <templates/hashmap.h>
#include <templates/pool.h>

#include <algorithm>

template <> tram::Pool<tram::Ext::Kitchensink::AnimationTable> tram::PoolProxy<tram::Ext::Kitchensink::AnimationTable>::pool("animatio pool", 100);

namespace tram::Ext::Kitchensink {

static Hashmap<AnimationTable*> sound_table_list("soundtbale name list", 200);

AnimationTable* AnimationTable::Find(name_t name) {
    AnimationTable* sound_table = sound_table_list.Find(name);
    
    if (!sound_table) {
        sound_table = PoolProxy<AnimationTable>::New(name);
        sound_table_list.Insert(name, sound_table);
    }
    
    return sound_table;
}

void AnimationTable::SwitchState(name_t state) {
    auto next_state = std::find_if(states.begin(), states.end(), [=](auto& f) { return f.name == state; });
    if (next_state == states.end()) {
        std::cout << "AnimationTable " << this->name << " state " << state << " not found!" << std::endl;
        assert(false);
    }
    
    auto next_state_space = std::find_if(state_spaces.begin(), state_spaces.end(), [=](auto& f) { return f.name == next_state->state_space; });
    
    // determine which states will be yeeted because of the new state
    std::vector<AnimStateSpace*> cancel_spaces;
    cancel_spaces.push_back(next_state_space.base());
    
    for (auto& space : state_spaces) {
        bool cancel = false;
        for (auto cancel_space : cancel_spaces) {
            if (cancel_space->name == space.parent) {
                cancel = true;
                break;
            }
        }
        if (cancel) cancel_spaces.push_back(&space);
    }
    
    // do outro transitions for yeetable state animations
    for (auto cancel_space : cancel_spaces) {
        if (!cancel_space->state) continue;
        
        auto state = std::find_if(states.begin(), states.end(), [=](auto& f) { return f.name == cancel_space->state; });
        
        bool faded_out = false;
        
        // check if there is a special outro transition
        for (auto& trans : transition_from) {
            if (trans.from_state != state->name) continue;
            if (trans.into_state != next_state->name) continue;
            
            faded_out = true;
            
            component->FadeOut(state->animation, trans.fade_time);
        }
        
        if (faded_out) continue;
        
        // otherwise use default
        for (auto& trans : transition_default) {
            if (trans.state != state->name) continue;
            
            faded_out = true;
            
            component->FadeOut(state->animation, trans.fade_out_time);
        }
        
        if (faded_out) continue;
        
        // if no default, then just cancel
        component->Stop(state->animation);
    }
    

    next_state_space->state = state;
    component->Play(next_state->animation,
                    next_state->repeats,
                    next_state->weight,
                    next_state->speed,
                    next_state->interpolate,
                    next_state->pause_last);
    
    name_t cancel_from;
    bool did_fade_in = false;
    
    // check if there is a special intro transition
    for (auto cancel_space : cancel_spaces) {
        auto prev_state = std::find_if(states.begin(), states.end(), [=](auto& f) { return f.name == cancel_space->state; });
        for (auto& trans : transition_into) {
            if (trans.from_state != prev_state->name) continue;
            if (trans.into_state != next_state->name) continue;
            
            if (cancel_from) {
                std::cout << "AnimationTable " << this->name << " transition " << cancel_from << " and " << prev_state->name << " conflict into " << next_state->name << std::endl;
                assert(false);
            } else {
                cancel_from = prev_state->name;
            }
            
            did_fade_in = true;
            
            component->FadeIn(next_state->animation, trans.fade_time);
        }
    }
    
    if (did_fade_in) return;
    
    // otherwise use default
    for (auto& trans : transition_default) {
        if (trans.state != next_state->name) continue;
        
        component->FadeIn(next_state->animation, trans.fade_out_time);
    }
}

void AnimationTable::AddStateSpace(name_t name, name_t parent) {
    auto ptr = std::find_if(states.begin(), states.end(), [=](auto& f) { return f.name == parent; });
    if (ptr == states.end()) {
        std::cout << "AnimationTable " << this->name << " parent state " << parent << " for " << name << " not found!" << std::endl;
        assert(false);
    }
    
    state_spaces.push_back({.name = name, .parent = parent});
}

void AnimationTable::AddState(name_t state, name_t space) {
    states.push_back({.name = state, .state_space = space});
}


void AnimationTable::SetStateAnimation(name_t state, name_t animation_name, uint32_t repeats, float weight, float speed, bool pause_last) {
    auto ptr = std::find_if(states.begin(), states.end(), [=](auto& f) { return f.name == state; });
    if (ptr == states.end()) {
        std::cout << "AnimationTable " << this->name << " state " << state << " not found!" << std::endl;
        assert(false);
    }
    ptr->animation = animation_name;
    ptr->repeats = repeats;
    ptr->weight = weight;
    ptr->speed = speed;
    ptr->interpolate = true;
    ptr->pause_last = pause_last;
}

void AnimationTable::SetStateNextState(name_t state, name_t next_state) {
    auto ptr = std::find_if(states.begin(), states.end(), [=](auto& f) { return f.name == state; });
    if (ptr == states.end()) {
        std::cout << "AnimationTable " << this->name << " state " << state << " not found!" << std::endl;
        assert(false);
    }
    ptr->next_state = next_state;
}


void AnimationTable::AddTransition(AnimStateTransitionFrom transition) {
    transition_from.push_back(transition);
}

void AnimationTable::AddTransition(AnimStateTransitionInto transition) {
    transition_into.push_back(transition);
}

void AnimationTable::AddTransition(AnimStateTransitionDefault transition) {
    transition_default.push_back(transition);
}

}
