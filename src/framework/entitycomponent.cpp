// TRAMWAY DRIFT AND DUNGEON EXPLORATION SIMULATOR 2022
// All rights reserved.

#include <framework/entitycomponent.h>

#include <templates/hashmap.h>

namespace tram {

static Hashmap<EntityComponent*> component_id_list("Component ID hashmap", 2000);

EntityComponent::EntityComponent() {
    id = GenerateID();
    
    component_id_list.Insert(id, this);
}

EntityComponent::~EntityComponent() {
    component_id_list.Insert(id, nullptr); // this eventually causes overflow
    // TODO: fix
}


EntityComponent* EntityComponent::Find (id_t component_id){
    return component_id_list.Find(component_id);
}

}