// TRAMWAY DRIFT AND DUNGEON EXPLORATION SIMULATOR 2022
// All rights reserved.

#ifndef TRAM_SDK_EXTENSIONS_DESIGN_ATTRIBUTES_H
#define TRAM_SDK_EXTENSIONS_DESIGN_ATTRIBUTES_H

#include <vector>

#include <framework/uid.h>

namespace tram::Ext::Design {

typedef int attribute;

enum Attributes : attribute {
    ATTRIBUTE_HITPOINTS,
    ATTRIBUTE_LAST
};

struct AttributeDefinition {
    name_t name;
    float min_value;
    float max_value;

    bool integer;
    bool linear_boost;
    bool stack_boost;
    
    static attribute Register(const AttributeDefinition& def);
    static void Replace(attribute attrib, const AttributeDefinition& def);
};


struct AttributeInstance {
    attribute type;
    float value;
    float max_value;
};

struct AttributeBoost {
    attribute type;

    int expires;            // how long until boost expires
    float boost;            // ammount of the boost
    bool boost_multiply;    // boost will be multiplied, otherwise added
    bool boost_once;        // boost will be applied once
};

struct AttributeBag {
    std::vector<AttributeInstance> attributes;
    std::vector<AttributeBoost> boosts;

    float GetAttribute(attribute type);
    const AttributeInstance& GetBaseAttribute(attribute type);
    void SetBaseAttribute(attribute type, const AttributeInstance& new_attrib);

    bool HasAttribute(attribute type);

    void RemoveBoosts();
    void ExpireBoosts();

    void ApplyBoost(const AttributeBoost& boost);
    void RemoveBoost(const AttributeBoost& boost);

    void Tick();
};


}

#endif // TRAM_SDK_EXTENSIONS_DESIGN_ATTRIBUTES_H