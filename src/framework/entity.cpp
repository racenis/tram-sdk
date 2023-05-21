// TRAMWAY DRIFT AND DUNGEON EXPLORATION SIMULATOR 2022
// All rights reserved.

#include <framework/entity.h>
#include <framework/worldcell.h>
#include <framework/serialization.h>
#include <framework/serialization/serialization.h>
#include <framework/file.h>

#include <templates/hashmap.h>

#include <iostream>
#include <unordered_map>

namespace tram {

struct EntityTypeInfo {
    Entity* (*constructor)(const SharedEntityData&, const SerializedFieldArray&) = nullptr;
    void (*destructor)(Entity*);
    const uint32_t* fields;
    size_t fieldcount;
};
    
struct SharedEntityData {
    uint64_t id;
    name_t name;
    vec3 position;
    quat rotation;
};

static Hashmap<Entity*> entity_id_list ("Entity ID hashmap", 500);
static Hashmap<Entity*> entity_name_list ("Entity name hashmap", 500);
static Hashmap<EntityTypeInfo> registered_entity_types ("Entity type hashmap", 50);

Entity::Entity (name_t name) {
    this->id = GenerateID();
    this->name = name;
    Register();
}

/*Entity::Entity (std::string_view& str) {
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
}*/

Entity::Entity(const SharedEntityData& shared_data) {
    id = shared_data.id;
    name = shared_data.name;
    location = shared_data.position;
    rotation = shared_data.rotation;
    
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

void Entity::RegisterType(name_t name, Entity* (*constr_func)(const SharedEntityData&, const SerializedFieldArray&), void (*destr_func)(Entity*), const uint32_t* fields, size_t fieldcount) {
    registered_entity_types.Insert(name, {constr_func, destr_func, fields, fieldcount});
}

void Entity::Register(){
    if (id) {
        entity_id_list.Insert(id, this);
    }

    if (name) {
        entity_name_list.Insert(name, this);
    }
}

Entity* Entity::Find (id_t entityID){
    return entity_id_list.Find(entityID);
}

Entity* Entity::Find(name_t entityName){
    return entity_name_list.Find(entityName);
}

/// Loads an Entity from a File.
Entity* Entity::Make(name_t type, File* file) {
    auto record = registered_entity_types.Find(type);
    
    if (!record.constructor) return nullptr;
    
    SharedEntityData shared_data {
        file->read_uint64(),
        file->read_name(),
        vec3 {file->read_float32(), file->read_float32(), file->read_float32()},
        vec3 {file->read_float32(), file->read_float32(), file->read_float32()}
    };
    
    std::vector<SerializedField> fields;
    
    // TODO: do not the the. is is the.
    
    // TODO: make fast; std::vector slow
    
    for (size_t i = 0; i < record.fieldcount; i++) {
        switch (record.fields[i]) {
            case TYPE_BOOL:     fields.push_back((bool) file->read_uint32()); break; // hehe
            case TYPE_INT:      fields.push_back(file->read_int32());   break;
            case TYPE_UINT:     fields.push_back(file->read_uint32());  break;
            case TYPE_FLOAT:    fields.push_back(file->read_float32()); break;
            case TYPE_NAME:     fields.push_back(file->read_name());    break;
            case TYPE_STRING:   fields.push_back(nullptr);              break; // TODO: fix
            case TYPE_INT8:     fields.push_back(file->read_int8());    break;
            case TYPE_INT16:    fields.push_back(file->read_int16());   break;
            case TYPE_INT32:    fields.push_back(file->read_int32());   break;
            case TYPE_INT64:    fields.push_back(file->read_int64());   break;
            case TYPE_UINT8:    fields.push_back(file->read_uint8());   break;
            case TYPE_UINT16:   fields.push_back(file->read_uint16());  break;
            case TYPE_UINT32:   fields.push_back(file->read_uint32());  break;
            case TYPE_UINT64:   fields.push_back(file->read_uint64());  break;
            case TYPE_FLOAT32:  fields.push_back(file->read_float32()); break;
            case TYPE_FLOAT64:  fields.push_back(file->read_float64()); break;
            default: assert(false);
        }
    }
    
    SerializedFieldArray field_array(fields.data(), fields.size());
    
    return record.constructor(shared_data, field_array);
}

/*Entity* Entity::Make (name_t type_name, std::string_view& params){
    Entity* entity = nullptr;
    
    auto constructor = entity_constructors.Find(type_name);
    
    if (constructor) {
        entity = constructor(params);
    }
    
    return entity;
}*/

}