
#include <extensions/kitchensink/inventory.h>

#include <templates/hashmap.h>

#include <framework/file.h>

#include <framework/entity.h>

using namespace tram;
template <> Pool<Ext::Kitchensink::ItemClass> PoolProxy<Ext::Kitchensink::ItemClass>::pool("itemclass pool", 500);
template <> Pool<Ext::Kitchensink::Inventory> PoolProxy<Ext::Kitchensink::Inventory>::pool("inventory pool", 500);

namespace tram::Ext::Kitchensink {

static Hashmap<ItemClass*> item_class_list("item_class_list list", 500);
static Hashmap<Inventory*> inventory_list("inventory_list list", 500);

struct InventoryFactoryInfo {
    InventoryManager*(*make)(void) = nullptr;
    void (*clear)(InventoryManager*) = nullptr;
};
static Hashmap<InventoryFactoryInfo*> inv_factory("inventoru_factory_info", 100);

void ItemClass::Register(ItemClass* item) {
    item_class_list.Insert(item->name, item);
}

ItemClass* ItemClass::Find(name_t name) {
    ItemClass* item_class = item_class_list.Find(name);
    
    if (!item_class) {
        item_class = PoolProxy<ItemClass>::New();
        item_class->name = name;
        item_class_list.Insert(UID(name), item_class);
    }
    
    return item_class;
}


InventoryManager* InventoryManager::New(name_t compartment) {
    auto info = inv_factory.Find(compartment);
    
    if (!info) {
        std::cout << "No InventoryManager found for " << compartment << "!" << std::endl;
        
        return new ListInventoryManager;
    }
    
    return info->make();
}

void InventoryManager::Delete(name_t compartment, InventoryManager* manager) {
    auto info = inv_factory.Find(compartment);
    
    if (!info) {
        delete manager;
    }
    
    return info->clear(manager);
}

void InventoryManager::Register(name_t compartment, InventoryManager*(*make)(void), void (*clear)(InventoryManager*)) {
    inv_factory.Insert(compartment, new InventoryFactoryInfo {.make = make,
                                                              .clear = clear});
}



int ListInventoryManager::AddItem(name_t item_class, int count) {
    int max_stack = ItemClass::Find(item_class)->stack;
    
    for (auto& item : items) {
        if (item.first != item_class) continue;
        
        int total_count = item.second + count;
        
        // max_stack negative value means that stack is unlimited
        if (max_stack < 0) {
            item.second = total_count;
            return count;
        } else if (max_stack > total_count) {
            int added = max_stack - item.second;
            item.second = max_stack;
            return added;
        } else {
            item.second = total_count;
            return count;
        }
    }
    
    if (count > max_stack) {
        items.push_back({item_class, max_stack});
        return max_stack;
    }
    
    items.push_back({item_class, count});
    return count;
}

int ListInventoryManager::RemoveItem(name_t item_class, int count) {
    for (auto& item : items) {
        if (item.first != item_class) continue;
        
        if (item.second <= count) {
            int over = count - item.second;
            std::erase_if(items, [=](auto& item){ return item.first = item_class;});
            return over;
        } else {
            item.second -= count;
            return count;
        }
    }
    
    return count;
}

int ListInventoryManager::GetItemCount(name_t item_class) {
    for (auto& item : items) {
        if (item.first != item_class) continue;
        return item.second;
    }
    
    return 0;
}

std::vector<name_t> ListInventoryManager::GetItems() {
    std::vector<name_t> new_items;
    new_items.reserve(items.size());
    
    for (auto& item: items) new_items.push_back(item.first);
    
    return new_items;
}

InventoryManager* Inventory::FindCompartment(name_t compartment) {
    if (!compartment) compartment = "default";
    
    for (auto& pair : compartments) {
        if (pair.first == compartment) return pair.second;
    }
    
    auto new_compartment = InventoryManager::New(compartment);
    compartments.push_back({compartment, new_compartment});
    
    return new_compartment;
}

int Inventory::AddItem(name_t item_class, int count) {
    auto item_info = ItemClass::Find(item_class);
    
    auto item_callback = item_info;
    while (!item_callback->OnAdded(item_info, this) && item_callback->base_class) {
        item_callback = ItemClass::Find(item_callback->base_class);
    }

    return FindCompartment(item_info->default_compartment)->AddItem(item_class, count);
}

int Inventory::RemoveItem(name_t item_class, int count) {
    auto item_info = ItemClass::Find(item_class);
    
    auto item_callback = item_info;
    while (!item_callback->OnRemoved(item_info, this) && item_callback->base_class) {
        item_callback = ItemClass::Find(item_callback->base_class);
    }

    return FindCompartment(item_info->default_compartment)->RemoveItem(item_class, count);
}

int Inventory::GetItemCount(name_t item_class) {
    auto item_info = ItemClass::Find(item_class);
    return FindCompartment(item_info->default_compartment)->GetItemCount(item_class);
}


bool Inventory::EquipItem(name_t item_class) {
    auto item_info = ItemClass::Find(item_class);
    auto item_compartment = FindCompartment(item_info->default_compartment);
    
    // check if has item
    if (item_compartment->GetItemCount(item_class) < 1) {
        return false;
    }
    
    // check if slot exists
    for (auto& slot : equipped) {
        if (slot.first != item_info->equipped_slot) continue;
        
        // unequip previous one
        auto prev_callback = ItemClass::Find(slot.second);;
        while (!prev_callback->OnUnequip(item_info, this) && prev_callback->base_class) {
            prev_callback = ItemClass::Find(prev_callback->base_class);
        }
        
        // equip this one
        auto next_callback = item_info;
        while (!next_callback->OnEquip(item_info, this) && next_callback->base_class) {
            next_callback = ItemClass::Find(next_callback->base_class);
        }
        
        return true;
    }
    
    // otherwise equip and install slot
    auto next_callback = item_info;
    while (!next_callback->OnEquip(item_info, this) && next_callback->base_class) {
        next_callback = ItemClass::Find(next_callback->base_class);
    }
    
    equipped.push_back({item_info->equipped_slot, item_class});
    
    return true;
}

bool Inventory::UnequipItem(name_t item_class) {
    for (auto& slot : equipped) {
        if (slot.second != item_class) continue;
        
        auto item_info = ItemClass::Find(item_class);
        auto callback = item_info;
        while (!callback->OnUnequip(item_info, this) && callback->base_class) {
            callback = ItemClass::Find(callback->base_class);
        }
        
        std::erase_if(equipped, [=](auto& slot) {return slot.second == item_class;});
        
        return true;
    }
    
    return false;
}

bool Inventory::UnequipSlot(name_t item_slot) {
    for (auto& slot : equipped) {
        if (slot.first != item_slot) continue;
        
        auto item_info = ItemClass::Find(slot.second);
        auto callback = item_info;
        while (!callback->OnUnequip(item_info, this) && callback->base_class) {
            callback = ItemClass::Find(callback->base_class);
        }
        
        std::erase_if(equipped, [=](auto& slot) {return slot.first == item_slot;});
        
        return true;
    }
    
    return false;
}

bool Inventory::IsEquippedItem(name_t item) {
    for (auto& entry : equipped) if (entry.second == item) return true;
    return false;
}

bool Inventory::IsEquippedSlot(name_t slot) {
    for (auto& entry : equipped) if (entry.first == slot) return true;
    return false;
}

name_t Inventory::GetEquippedItem(name_t slot) {
    for (auto& entry : equipped) if (entry.first == slot) return entry.second;
    return "none";
}

Inventory* Inventory::Find(Entity* entity) {
    Inventory* inventory = inventory_list.Find(entity->GetID());
    
    if (!inventory) {
        inventory = PoolProxy<Inventory>::New(entity->GetID());
        inventory_list.Insert(entity->GetID(), inventory);
    }
    
    return inventory;
}


void Inventory::LoadFromDisk(const char* filename) {
    char path [100] = "data/";
    strcat(path, filename);
    strcat(path, ".item");

    File file(path, MODE_READ);

    if (!file.is_open()) {
        std::cout << "Can't open inventory file '" << path << "'" << std::endl;
        abort();
    }

    name_t file_type = file.read_name();

    if (file_type != "ITEMv1") {
        std::cout << "Invalid item file type " << path << std::endl;
        abort();
    }
    
    std::cout << "Loading: " << filename << std::endl;

    while (file.is_continue()) {
        name_t record = file.read_name();
        
        if (record == "item-class") {
            ItemClass* item = ItemClass::Find(file.read_name());
            
            item->base_class = file.read_name();
            item->equipped_slot = file.read_name();
            item->default_compartment = file.read_name();
        } else if (record == "world-display") {
            ItemClass* item = ItemClass::Find(file.read_name());
            
            item->viewmodel = file.read_name();
            item->worldmodel = file.read_name();
        } else if (record == "item-layout") {
            ItemClass* item = ItemClass::Find(file.read_name());
            
            item->width = file.read_int32();
            item->height  = file.read_int32();
            
            item->stack  = file.read_int32();
            
            item->weight  = file.read_float32();
            item->value  = file.read_int32();
        } else if (record == "item-attribute") {
            ItemClass* item = ItemClass::Find(file.read_name());
            
            Attribute new_attribute;
            new_attribute.name = file.read_name();
            new_attribute.base_value = file.read_float32();
            
            item->attributes.push_back(new_attribute);
        } else if (record == "item-effect") {
            ItemClass* item = ItemClass::Find(file.read_name());
            
            Effect new_effect;
            new_effect.attribute = file.read_name();
            new_effect.value = file.read_float32();
            new_effect.name = file.read_name();
            new_effect.tag = file.read_name();
            name_t effect_type = file.read_name();
            //new_effect.type = file.read_name();
            new_effect.time = file.read_float32();
            
            if (effect_type == "raise-attribute") {
                new_effect.type = EFFECT_RAISE_BASE;
            } else if (effect_type == "negate-change") {
                new_effect.type = EFFECT_NEGATE_CHANGE;
            } else {
                std::cout << "Unrecognized effect type " << effect_type <<" in file: " << filename << std::endl;
            }
            
            item->effects.push_back(new_effect);
        } else {
            std::cout << "Unrecognized record " << record <<" in file: " << filename << std::endl;
        }
    }
}

}