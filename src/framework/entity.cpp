// TRAMWAY DRIFT AND DUNGEON EXPLORATION SIMULATOR 2022
// All rights reserved.

#include <framework/entity.h>
#include <framework/worldcell.h>
#include <framework/serializeddata.h>

#include <templates/hashmap.h>

#include <iostream>
#include <unordered_map>

namespace tram {

static Hashmap<Entity*> entity_id_list ("Entity ID hashmap", 500);
static Hashmap<Entity*> entity_name_list ("Entity name hashmap", 500);
static Hashmap<Entity* (*)(std::string_view& params)> entity_constructors ("Entity constructor hasmap", 50);

Entity::Entity (name_t name) {
    this->id = GenerateID();
    this->name = name;
    Register();
}

Entity::Entity (std::string_view& str) {
    id = SerializedEntityData::Field<uint64_t>().FromString(str);
    name = SerializedEntityData::Field<name_t>().FromString(str);

    location.x = SerializedEntityData::Field<float>().FromString(str);
    location.y = SerializedEntityData::Field<float>().FromString(str);
    location.z = SerializedEntityData::Field<float>().FromString(str);

    float rx = SerializedEntityData::Field<float>().FromString(str);
    float ry = SerializedEntityData::Field<float>().FromString(str);
    float rz = SerializedEntityData::Field<float>().FromString(str);

    rotation = glm::quat(glm::vec3(rx, ry, rz));

    if (!id) {
        is_serializable = false;
        id = GenerateID();
    }
    
    Register();
}

Entity::~Entity() {
    if (cell) cell->RemoveEntity(this);

    if (id) {
        if (entity_id_list.Find(id)) {
            entity_id_list.Insert(id, nullptr);
        }
    }

    if (name) {
        if (entity_name_list.Find(name)) {
            entity_name_list.Insert(name, nullptr);
        }
    }
}

void Entity::CheckTransition() {
#ifndef ENGINE_EDITOR_MODE
    if (!cell) return;

    WorldCell* into = cell->FindTransition(location);

    if (!into) return;
    
    std::cout << name << " transitioned into " << into->GetName() << std::endl;
    
    cell->RemoveEntity (this);
    into->AddEntity (this);
    
#endif // ENGINE_EDITOR_MODE
}

void Entity::RegisterType(name_t name, Entity* (*constr_func)(std::string_view& params)){
    entity_constructors.Insert(name, constr_func);
}

void Entity::Register(){
    if (id) {
        entity_id_list.Insert(id, this);
    }

    if (name) {
        entity_name_list.Insert(name, this);
    }
}

Entity* Entity::Find (uint64_t entityID){
    return entity_id_list.Find(entityID);
}

Entity* Entity::Find(name_t entityName){
    return entity_name_list.Find(entityName);
}

Entity* Entity::Make (name_t type_name, std::string_view& params){
    Entity* entity = nullptr;
    
    auto constructor = entity_constructors.Find(type_name);
    
    if (constructor) {
        entity = constructor(params);
    }
    
    return entity;
}

}