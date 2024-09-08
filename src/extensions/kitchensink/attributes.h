// TRAMWAY DRIFT AND DUNGEON EXPLORATION SIMULATOR 2022
// All rights reserved.

#ifndef TRAM_SDK_EXTENSIONS_KITCHENSINK_ATTRIBUTES_H
#define TRAM_SDK_EXTENSIONS_KITCHENSINK_ATTRIBUTES_H

#include <vector>

#include <framework/uid.h>

namespace tram::Ext::Kitchensink {

enum : int {
    ATTRIBUTE_MODIFIER_ADD,
    ATTRIBUTE_MODIFIER_MULTIPLY,
};

struct Attribute {
    name_t name;
    float value;
};

struct AttributeModifier {
    name_t name;
    name_t tag;
    float value;
    int flags;
    float time;
};

struct AttributeContainer {
    float GetAttribute(name_t attribute);
    float GetBaseAttribute(name_t attribute);
    
    void SetAttribute(name_t attribute, float value);
    
    void ApplyModifier(AttributeModifier modifier);
    void RemoveModifier(name_t tag);

    bool HasAttribute(name_t type);

    void Tick();
    
    std::vector<Attribute> attributes;
    std::vector<AttributeModifier> modifiers;
};


}

#endif // TRAM_SDK_EXTENSIONS_KITCHENSINK_ATTRIBUTES_H