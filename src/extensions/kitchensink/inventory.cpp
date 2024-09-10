
#include <extensions/kitchensink/inventory.h>

#include <templates/hashmap.h>

using namespace tram;
template <> Pool<Ext::Kitchensink::ItemClass> PoolProxy<Ext::Kitchensink::ItemClass>::pool("itemclass pool", 500);
template <> Pool<Ext::Kitchensink::Inventory> PoolProxy<Ext::Kitchensink::Inventory>::pool("inventory pool", 500);

namespace tram::Ext::Kitchensink {

static Hashmap<ItemClass*> item_class_list("item_class_list list", 500);
static Hashmap<Inventory*> inventory_list("inventory_list list", 500);

ItemClass* ItemClass::Find(name_t name) {
    ItemClass* item_class = item_class_list.Find(name);
    
    if (!item_class) {
        item_class = PoolProxy<ItemClass>::New(name);
        item_class_list.Insert(UID(name), item_class);
    }
    
    return item_class;
}

int Inventory::AddItem(const ItemInstance& item) {
    items.push_back(item);
    return item.count;
}

int Inventory::AddItem(name_t item_class, int count) {
    int max_stack = ItemClass::Find(item_class)->stack;
        
    for (auto& item : items) {
        if (item.item_class != item_class) continue;
        
        int total_count = item.count + count;
        
        if (max_stack > total_count) {
            item.count = max_stack;
            return total_count - max_stack;
        } else {
            item.count = total_count;
            return count;
        }
    }
    
    // TODO: add a check for if can pickup item! i.e. fits in grid
    
    items.push_back({.item_class = item_class, .count = count});
    return count;
}

int Inventory::RemoveItem(name_t item_class, int count) {
    for (auto& item : items) {
        if (item.item_class != item_class) continue;
        
        if (item.count <= count) {
            int over = count - item.count;
            std::erase_if(items, [=](auto& item){ return item.item_class = item_class;});
            return over;
        } else {
            item.count -= count;
            return count;
        }
    }
    return count;
}

int Inventory::GetItemCount(name_t item_class) {
    for (auto& item : items) {
        if (item.item_class != item_class) continue;
        return item.count;
    }
    return 0;
}

Inventory* Inventory::Find(id_t id) {
    Inventory* inventory = inventory_list.Find(id);
    
    if (!inventory) {
        inventory = PoolProxy<Inventory>::New();
        inventory_list.Insert(id, inventory);
    }
    
    return inventory;
}

}