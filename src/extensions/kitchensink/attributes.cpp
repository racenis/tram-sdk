#include <extensions/kitchensink/attributes.h>

#include <framework/core.h>

#include <algorithm>

namespace tram::Ext::Kitchensink {

float AttributeContainer::GetAttribute(name_t name) {
    for (auto& attribute : attributes) {
        if (attribute.name == name) return attribute.effective_value;
    }
    return 0.0f;
}

float AttributeContainer::GetAttributeBase(name_t name) {
    for (auto& attribute : attributes) {
        if (attribute.name == name) return attribute.base_value;
    }
    return 0.0f;
}

float AttributeContainer::GetAttributeRemaining(name_t name) {
    for (auto& attribute : attributes) {
        if (attribute.name == name) return attribute.remaining_value;
    }
    return 0.0f;
}

void AttributeContainer::SetAttribute(name_t name, float base_value, float remaining) {
    for (auto& attribute : attributes) {
        if (attribute.name != name) continue;
        attribute.base_value = base_value;
        attribute.remaining_value = remaining;
        return;
    }
    attributes.push_back({.name = name,
                          .base_value = base_value,
                          .remaining_value = remaining,
                          .effective_value = base_value});
}

void AttributeContainer::ApplyEffect(Effect effect) {
    effects.push_back(effect);
}

void AttributeContainer::RemoveEffect(name_t tag) {
    std::erase_if(effects, [=](auto& value){return value.tag == tag;});
}

bool AttributeContainer::HasAttribute(name_t type) {
    return std::find_if(attributes.begin(), attributes.end(), [=](auto value){return value.name == type;}) != attributes.end();
}

void AttributeContainer::Tick() {
    for (auto it = effects.begin(); it != effects.end(); it++) {
        // TODO: whatever
        
        // if time == 0.0f then apply full effect
        // other wise tick down
        
        it->time -= GetDeltaTime();
        
        if (0.0f >= it->time) {
            
        }
    }
}
    
}