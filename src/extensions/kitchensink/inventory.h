// TRAMWAY DRIFT AND DUNGEON EXPLORATION SIMULATOR 2022
// All rights reserved.

#ifndef TRAM_SDK_EXTENSIONS_DESIGN_INVENTORY_H
#define TRAM_SDK_EXTENSIONS_DESIGN_INVENTORY_H

#include <extensions/design/attributes.h>

namespace tram::Ext::Design {

typedef int item_class;

void RegisterItemAction(name_t name, void (*func)());

struct ItemProperty {
    enum Type {
        ATTRIBUTE_BOOST,
        FLAG,
    };
    
    Type type;
    
    union {
        AttributeBoost attribute_boost;
        name_t flag;
    };
};


struct ItemClass {
    name_t name;
    
    name_t viewmodel;
    
    name_t viewmodel_animation;
    name_t primary_action_animation;
    name_t secondary_action_animation;
    
    name_t primary_action;
    name_t secondary_action;
    
    bool draw_hands = false;
    
    int sprite = -1;
    int width = 1, height = 1;
    int stack = 1;
    
    std::vector<AttributeBoost> base_boosts;
    
    item_class Register(name_t name, const ItemClass& def);
};

struct ItemInstance {
    int x = 0, y = 0;
    int count = 1;

    item_class item_class;
};

struct Inventory {
    std::vector<ItemInstance> items;
    
    int holding = -1;
    int width = 1, height = 1;
    
    bool allow_overlap = true;
    
    void AddItem(const ItemInstance& item);
    void AddItem(item_class item, int count);
    void RemoveItem(item_class item, int count);
};




}

#endif // TRAM_SDK_EXTENSIONS_DESIGN_INVENTORY_H