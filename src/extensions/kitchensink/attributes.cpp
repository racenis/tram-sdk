#include <extensions/kitchensink/attributes.h>

#include <framework/core.h>

#include <algorithm>

namespace tram::Ext::Kitchensink {

float AttributeContainer::GetAttribute(name_t name) {
    float value = GetBaseAttribute(name);
    for (auto& modifier : modifiers) {
        if (modifier.name != name) continue;
        
        // do some logic in here...
        
        if (modifier.flags & ATTRIBUTE_MODIFIER_ADD) {
            value += modifier.value;
        }
        
        if (modifier.flags & ATTRIBUTE_MODIFIER_MULTIPLY) {
            value *= modifier.value;
        }
    }
    return value;
}

float AttributeContainer::GetBaseAttribute(name_t name) {
    for (auto& attribute : attributes) {
        if (attribute.name == name) return attribute.value;
    }
    return 0.0f;
}

void AttributeContainer::SetAttribute(name_t name, float value) {
    for (auto& attribute : attributes) {
        if (attribute.name != name) continue;
        attribute.value = value;
        return;
    }
    attributes.push_back({.name = name, .value = value});
}

void AttributeContainer::ApplyModifier(AttributeModifier modifier) {
    modifier.time += GetTickTime();
    modifiers.push_back(modifier);
}

void AttributeContainer::RemoveModifier(name_t tag) {
    std::erase_if(modifiers, [=](auto& value){return value.tag == tag;});
}

bool AttributeContainer::HasAttribute(name_t type) {
    return std::find_if(modifiers.begin(), modifiers.end(), [=](auto value){return value.name == type;}) != modifiers.end();
}

void AttributeContainer::Tick() {
    std::erase_if(modifiers, [](auto& value){return value.time < GetTickTime();});
}
    
}