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
    EFFECT_RAISE_CONSTANT,
    EFFECT_NEGATE_CHANGE,
};

struct Attribute {
    name_t name;
    float base_value;
    float affected_value;
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
    virtual void Recalculate(Attribute&, const AttributeContainer&);
    
    name_t GetName() { return name; }
    static void Register(AttributeInfo*);
    static AttributeInfo* Find(name_t);
protected:
    name_t name;
};

struct AttributeContainer {
    float GetAttribute(name_t attribute);
    float GetAttributeBase(name_t attribute);
    
    void SetAttribute(name_t attribute, float base_value);
    
    void ApplyEffect(Effect effect);
    void RemoveEffect(name_t tag);

    bool HasAttribute(name_t type);

    void Tick();
    
    static AttributeContainer* Find(Entity*);
    static AttributeContainer* Find(id_t);
    static void LoadFromDisk(const char*);
    
    // maybe instead use global clock???
    float last_tick = 0.0f;
    
    std::vector<Attribute> attributes;
    std::vector<Effect> effects;
};


}

#endif // TRAM_SDK_EXTENSIONS_KITCHENSINK_ATTRIBUTES_H