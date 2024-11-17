#include <extensions/kitchensink/attributes.h>

#include <framework/core.h>
#include <framework/file.h>
#include <framework/entity.h>

#include <templates/hashmap.h>
#include <templates/pool.h>

#include <algorithm>

const size_t ATTRIBUTE_CONTAINER_COUNT = 50;

namespace tram {

template<> Pool<Ext::Kitchensink::AttributeContainer> PoolProxy<Ext::Kitchensink::AttributeContainer>::pool("Ext::Kitchensink::AttributeContainer Pool", ATTRIBUTE_CONTAINER_COUNT);

}

namespace tram::Ext::Kitchensink {

Hashmap<AttributeContainer*> containers("Ext::Kitchensink::AttributeContainer Hashmap", ATTRIBUTE_CONTAINER_COUNT);
    
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

AttributeContainer* AttributeContainer::Find(Entity* entity) {
    return AttributeContainer::Find(entity->GetID());
}

AttributeContainer* AttributeContainer::Find(id_t entity_id) {
    AttributeContainer* container = containers.Find(entity_id);
    
    if (!container) {
        container = PoolProxy<AttributeContainer>::New();
        containers.Insert(entity_id, container);
    }
    
    return container;
}

void AttributeContainer::LoadFromDisk(const char* filename) {
    char path [100] = "data/";
    strcat(path, filename);
    strcat(path, ".attrib");

    File file (path, MODE_READ);

    if (!file.is_open()) {
        std::cout << "Can't open attribute file '" << path << "'" << std::endl;
        abort();
    }

    name_t file_type = file.read_name();

    if (file_type != "ATTRIBv1") {
        std::cout << "Invalid attribute file type " << path << std::endl;
        abort();
    }
    
    std::cout << "Loading: " << filename << std::endl;

    while (file.is_continue()) {
        auto record = file.read_name();
        
        if (record == "attribute") {
            AttributeContainer* container = AttributeContainer::Find(file.read_uint32());
            
            Attribute new_attribute;
            
            new_attribute.name = file.read_name();
            new_attribute.base_value = file.read_float32();
            new_attribute.remaining_value = file.read_float32();
            new_attribute.effective_value = file.read_float32();

            container->attributes.push_back(new_attribute);

        } else if (record == "effect") {
            AttributeContainer* container = AttributeContainer::Find(file.read_uint32());
            
            Effect new_effect;
            
            new_effect.attribute = file.read_name();
            new_effect.name = file.read_name();
            new_effect.tag = file.read_name();
            new_effect.value = file.read_float32();
            new_effect.type = file.read_int32();
            new_effect.flags = file.read_int32();
            new_effect.time = file.read_float32();
            
            container->effects.push_back(new_effect);
            
        } else {
            std::cout << "unknown attribute record: " << record << std::endl;
            abort();
        }
    }
}

}