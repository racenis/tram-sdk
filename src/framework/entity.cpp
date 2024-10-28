// TRAMWAY DRIFT AND DUNGEON EXPLORATION SIMULATOR 2022
// All rights reserved.

#include <framework/entity.h>
#include <framework/worldcell.h>
#include <framework/serialization.h>
#include <framework/serialization.h>
#include <framework/file.h>

#include <templates/hashmap.h>

#include <iostream>
#include <unordered_map>

#include <algorithm>

namespace tram {

struct EntityTypeInfo {
    Entity* (*constructor)(const SharedEntityData&, const ValueArray&) = nullptr;
    void (*destructor)(Entity*);
    const uint32_t* fields;
    size_t fieldcount;
};
    
struct SharedEntityData {
    uint64_t id;
    name_t name;
    uint32_t flags;
    vec3 position;
    quat rotation;
};

static Hashmap<Entity*> entity_id_list ("Entity ID hashmap", 5000);
static Hashmap<Entity*> entity_name_list ("Entity name hashmap", 500);
static Hashmap<EntityTypeInfo> registered_entity_types ("Entity type hashmap", 50);

Entity::Entity() {
    this->id = GenerateID();
    Register();
}

Entity::Entity(name_t name) {
    this->id = GenerateID();
    this->name = name;
    Register();
}

Entity::Entity(const SharedEntityData& shared_data) {
    id = shared_data.id;
    name = shared_data.name;
    flags = shared_data.flags;
    location = shared_data.position;
    rotation = shared_data.rotation;
    
    if (!id) {
        flags |= NON_SERIALIZABLE;
        id = GenerateID();
    }
    
    Register();
}

Entity::~Entity() {
    if (cell) cell->Remove(this);

    if (id) {
        if (entity_id_list.Find(id)) {
            entity_id_list.Insert(id, nullptr); // this eventually causes overflow
            // TODO: fix
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
    
    cell->Remove(this);
    into->Add(this);
    
#endif // ENGINE_EDITOR_MODE
}

void Entity::RegisterType(name_t name, Entity* (*constr_func)(const SharedEntityData&, const ValueArray&), void (*destr_func)(Entity*), const uint32_t* fields, size_t fieldcount) {
    registered_entity_types.Insert(name, {constr_func, destr_func, fields, fieldcount});
}

void Entity::RegisterType(name_t name, Entity* (*constr_func)(const SharedEntityData&, const ValueArray&), void (*destr_func)(Entity*), std::initializer_list<FieldInfo> fields) {
    uint32_t* field_types = new uint32_t[fields.size()];
    
    for (auto& field : fields) {
        field_types[field.field_id] = field.field_type;
    }
    
    registered_entity_types.Insert(name, {constr_func, destr_func, field_types, fields.size()});
}

void Entity::Register() {
    if (id) {
        entity_id_list.Insert(id, this);
    }

    if (name) {
        entity_name_list.Insert(name, this);
    }
}

Entity* Entity::Find (id_t entityID) {
    return entity_id_list.Find(entityID);
}

Entity* Entity::Find(name_t entityName) {
    return entity_name_list.Find(entityName);
}

static std::vector<Entity*> update_list;

/// Adds entity to update list.
/// After being added to the update list, the entity's Update() method will be
/// called every time UpdateFromList() is called.
void Entity::AddUpdate() {
    update_list.push_back(this);
}

/// Remove entity from the update list.
void Entity::RemoveUpdate() {
    update_list.erase(std::find(update_list.begin(), update_list.end(), this));
}

/// Updates all of the entities in the update list.
void Entity::UpdateFromList() {
    for (auto it : update_list) {
        it->Update();
    }
}

/// Loads an Entity from a File.
Entity* Entity::Make(name_t type, File* file) {
    auto record = registered_entity_types.Find(type);
    
    if (!record.constructor) return nullptr;
    
    SharedEntityData shared_data {
        file->read_uint64(),
        file->read_name(),
        file->read_uint32(),
        vec3 {file->read_float32(), file->read_float32(), file->read_float32()},
        vec3 {file->read_float32(), file->read_float32(), file->read_float32()}
    };
    
    std::vector<Value> fields;
    
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
            case TYPE_VEC2:     fields.push_back(vec2{file->read_float32(),      // evil formatting. do not DO NOT
                                file->read_float32()});                 break;
            case TYPE_VEC3:     fields.push_back(vec3{file->read_float32(), 
                                file->read_float32(), file->read_float32()}
                                );                                      break;
            case TYPE_VEC4:     fields.push_back(vec4{file->read_float32(), 
                                file->read_float32(), file->read_float32(),
                                file->read_float32()});                 break;
            case TYPE_QUAT:     fields.push_back(quat{file->read_float32(),
                                file->read_float32(), file->read_float32(),
                                file->read_float32()});                 break;
            default: assert(false);
        }
    }
    
    ValueArray field_array(fields.data(), fields.size());
    
    return record.constructor(shared_data, field_array);
}

}