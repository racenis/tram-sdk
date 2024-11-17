// TRAMWAY DRIFT AND DUNGEON EXPLORATION SIMULATOR 2022
// All rights reserved.

#ifndef TRAM_SDK_EXTENSIONS_KITCHENSINK_ATTRIBUTES_H
#define TRAM_SDK_EXTENSIONS_KITCHENSINK_ATTRIBUTES_H

#include <vector>

#include <framework/core.h>
#include <framework/uid.h>

namespace tram {
    class Entity;
}

namespace tram::Ext::Kitchensink {

enum : int {
    EFFECT_RAISE_BASE,
    EFFECT_RAISE_REMAINING,
    EFFECT_RAISE_EFFECTIVE,
    
    EFFECT_NEGATE_CHANGE,
};

struct Attribute {
    name_t name;
    float base_value;
    float remaining_value;
    float effective_value;
};

struct Effect {
    name_t attribute;
    name_t name;
    name_t tag;
    float value;
    int type;
    int flags;
    float time;
};

struct AttributeContainer;

class AttributeInfo {
public:
    virtual void CalculateEffective(Attribute&, const AttributeContainer&) = 0;
    name_t GetName() { return name; }
    static void Register(AttributeInfo*);
    static AttributeInfo* Find(name_t);
protected:
    name_t name;
};

struct AttributeContainer {
    float GetAttribute(name_t attribute);
    
    float GetAttributeBase(name_t attribute);
    float GetAttributeRemaining(name_t attribute);
    
    void SetAttribute(name_t attribute, float base_value, float remaining);
    
    void ApplyEffect(Effect effect);
    void RemoveEffect(name_t tag);

    bool HasAttribute(name_t type);

    void Tick();
    
    static AttributeContainer* Find(Entity*);
    static AttributeContainer* Find(id_t);
    static void LoadFromDisk(const char*);
    
    std::vector<Attribute> attributes;
    std::vector<Effect> effects;
};


}

#endif // TRAM_SDK_EXTENSIONS_KITCHENSINK_ATTRIBUTES_H