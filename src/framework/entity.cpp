// Tramway Drifting and Dungeon Exploration Simulator SDK Runtime

#include <framework/entity.h>
#include <framework/worldcell.h>
#include <framework/serialization.h>
#include <framework/serialization.h>
#include <framework/file.h>
#include <framework/logging.h>

#include <templates/hashmap.h>

#include <config.h>

#include <iostream>
#include <unordered_map>

#include <algorithm>

/**
 * @class tram::Entity framework/entity.h <framework/entity.h>
 * 
 * Entity base class.
 * 
 * Provides API for entities and some basic functionality.
 * 
 * @see https://racenis.github.io/tram-sdk/documentation/framework/entity.html
 */
 
 /**
 * @fn void tram::Entity::Load()
 * 
 * Loads an entity.
 * 
 * Usually the Load() method for an entity type will set up its components,
 * event listeners, etc. After being loaded, the entity will be ready for
 * simulation.
 * 
 * When implementing this method, make sure to flag the entity with the `LOADED`
 * flag, so that world streaming system doesn't try to load your entity twice.
 * 
 * @fn void tram::Entity::Unload()
 * 
 * Unloads an entity.
 * 
 * Usually the Load() method for an entity type will destroy its its components,
 * event listeners, etc. After being unloaded, the entity have been removed from
 * the simulation.
 * 
 * When implementing this method, make sure to clear your entity's LOADED flag,
 * so that world streaming system can load the entity again.
 * 
 * @fn WorldCell* tram::Entity::GetCell() 
 * 
 * Returns the entity's parent cell.
 * 
 * If an entity is inside a WorldCell, this method will return a pointer to it,
 * otherwise it will return a `nullptr`.
 * 
 * @fn WorldCell* tram::Entity::IsLoaded()
 * 
 * Returns the entity load status.
 * 
 * @fn WorldCell* tram::Entity::IsAutoLoad()
 * 
 * Returns the auto load flag status.
 * 
 * By default all entities are automatically loaded, i.e. when the WorldCell in
 * which they reside is loaded. Sometimes you might want to override this, so
 * the automatic loading disablement flag is provided.
 * 
 * @fn WorldCell* tram::Entity::IsPersistent()
 * 
 * Returns the persistence status of an entity.
 * 
 * Entities that have been marked as non-persistent will be yeeted when their
 * parent cell is unloaded, instead of the usual unloading.
 * 
 * This is useful when creating small decoration entities, e.g. debris and
 * giblets, which don't need to be saved.
 * 
 * @fn WorldCell* tram::Entity::IsDeleted()
 * 
 * Returns the entity deletion status.
 * 
 * When an entity is yeeted, it is marked as deleted. They continue existing for
 * some time until the Entity::Update() static method is called and they are
 * destructed.
 * 
 * Even then, entities that have been loaded from the disk will continue
 * existing, so that their status as being deleted can be written back to disk.
 * 
 * This also allows to ressurect them, if the need arises.
 * 
 * @fn void tram::Entity::UpdateParameters()
 * 
 * Called when Entity::UpdateTransform is called.
 * 
 * From the runtime's side, only the Physics system will call the
 * Entity::UpdateTransform() method, and through it, this method.
 * 
 * Usually this method is used to forward the entity's new position and rotation 
 * to its RenderComponent, if it has one.
 * 
 * If the entity has a PhysicsComponent, its position or rotation is never
 * updated from this method.
 * 
 * @fn void tram::Entity::SetParameters()
 * 
 * Called when either Entity::SetLocation or Entity::SetRotation is called.
 * 
 * Usually this method is used to relocate or rerotate an entity.
 * 
 * If the entity has a PhysicsComponent, its position or rotation is usually set
 * from this method, otherwise in the next Physics update step, the entity's
 * previous rotation and location will be restored.
 * 
 * @fn void tram::Entity::GetType()
 * 
 * Returns the type of the entity.
 * 
 * Usually this method returns the same name that is used to
 * Entity::RegisterType() its type.
 * 
 * @fn void tram::Entity::MessageHandler(Message& msg)
 * 
 * Allows entities to receive messages.
 * 
 * Whenever an entity is sent a Message, once the Message::Dispatch() function
 * has been called, the entity will gets the message passed to it through this
 * method.
 * 
 * @fn void tram::Entity::EventHandler(Event &event)
 * 
 * Allows entities to receive events.
 * 
 * Whenever an entity registers an event, an event of the same type as the event
 * listener is posted and Event::Dispatch() has been called, the entity will
 * receive the event through this method.
 * 
 */
 
 /**
 * @typedef Entity* (*entity_make)(const SharedEntityData&, const ValueArray&)
 * 
 * Entity construction function.
 * 
 * This function takes in parameters and should return a newly constructed
 * entity. Internally it could be just calling `new` operator, or maybe it can
 * use some kind of other memory allocation for the entity.
 * 
 * Take a look at Entity::RegisterType() function to see what the function
 * should do with its parameters.
 */
 
  /**
 * @typedef void (*entity_clear)(Entity*)
 * 
 * Entity destruction function.
 * 
 * This function takes in an entity and destructs it. Internally it could be
 * just calling `delete` operator, or maybe it can use some other kind of memory
 * allocation for the entity.
 */
 
 /**
 * @struct tram::SharedEntityData framework/entity.h <framework/entity.h>
 * 
 * Basic Entity parameters.
 * 
 * Used for Entity serialization. This struct contains the parameters that every
 * Entity type shares in common.
 */

namespace tram {

struct EntityTypeInfo {
    Entity* (*constructor)(const SharedEntityData&, const ValueArray&) = nullptr;
    void (*destructor)(Entity*) = nullptr;
    const Entity::FieldInfo* fields = nullptr;
    size_t fieldcount = 0;
};

static Hashmap<Entity*> entity_id_list("Entity ID hashmap", ENTITY_LIMIT);
static Hashmap<Entity*> entity_name_list("Entity name hashmap", NAMED_ENTITY_LIMIT);
static Hashmap<EntityTypeInfo> registered_entity_types("Entity type hashmap", ENTITY_TYPE_LIMIT);

/// Creates an unnamed entity with a random ID.
Entity::Entity() {
    this->id = GenerateID();
    Register();
}

/// Creates a named entity with a random ID.
Entity::Entity(name_t name) {
    this->id = GenerateID();
    this->name = name;
    Register();
}

/// Creates an entity from a SharedEntityData.
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
    
    flags |= LOADED_FROM_DISK;
    
    Register();
}

/// Destructs the entity.
/// This automatically also removes the entity from its parent cell, and also
/// unregisters it.
Entity::~Entity() {
    if (cell) cell->Remove(this);
    
    Unregister();
}

/// Check whether an entity has entered a different cell.
void Entity::CheckTransition() {
#ifndef ENGINE_EDITOR_MODE
    if (!cell) return;

    WorldCell* into = cell->FindTransition(location);

    if (!into) return;
    
    Log(Severity::INFO, System::CORE, "Entity {} transitioned into {}", name, into->GetName());

    cell->Remove(this);
    into->Add(this);
    
    cell = into;
    
#endif // ENGINE_EDITOR_MODE
}

static void CheckEntityTypeDuplicate(name_t name, const Entity::FieldInfo* fields, size_t count) {
    if (!registered_entity_types.Exists(name)) return;
    
    Log(Severity::WARNING, System::CORE, "Entity type named {} already exists, overriding", name);
    
    const auto existing = registered_entity_types.Find(name);
    
    if (existing.fieldcount != count) {
        Log(Severity::CRITICAL_ERROR, System::CORE, "Entity type {} override {} -> {} field count", name, existing.fieldcount, count);
    }
    
    for (size_t i = 0; i < count; i++) {
        if (existing.fields[i].field_type != fields[i].field_type) {
            Log(Severity::CRITICAL_ERROR, System::CORE, "Entity type {} override, parameter {} type {} -> {}", name, i, TypeToString(existing.fields[i].field_type), TypeToString(fields[i].field_type));
        }
    }
}

/// Registers a new entity type.
/// @deprecated Use other RegisterType() methods instead.
void Entity::RegisterType(name_t name, entity_make constr_func, entity_clear destr_func, const Type* fields, size_t fieldcount) {
    FieldInfo* field_copy = nullptr;
    
    if (fields && fieldcount) {
        field_copy = new FieldInfo[fieldcount];
        
        for (size_t i = 0; i < fieldcount; i++) {
            field_copy[i].field_id = i;
            field_copy[i].field_type = fields[i];
            field_copy[i].field_flags = 0;
        }
    }
    
    CheckEntityTypeDuplicate(name, field_copy, fieldcount);
    
    registered_entity_types.Insert(name, {constr_func, destr_func, field_copy, fieldcount});
}

/// Registers a new entity type.
/// @param name Name of the entity, should be the same as what its GetType()
///                method returns.
/// @param constr_func This function will be called when a new entity of this type is requested, e.g. when deserializing a worldcell file.
/// @param destr_func This function will be called when an entity of this
/// type is yeeted.
/// @param fields Pointer to an array of serialization fields.
/// @param fieldcount Size of the `fields` array.
void Entity::RegisterType(name_t name, entity_make constr_func, entity_clear destr_func, const FieldInfo* fields, size_t fieldcount) {
    FieldInfo* field_copy = nullptr;
    
    if (fields && fieldcount) {
        field_copy = new FieldInfo[fieldcount];
        
        for (size_t i = 0; i < fieldcount; i++) {
            field_copy[fields[i].field_id] = fields[i];
        }
    }
    
    CheckEntityTypeDuplicate(name, field_copy, fieldcount);
    
    registered_entity_types.Insert(name, {constr_func, destr_func, field_copy, fieldcount});
}

/// Registers a new entity type.
/// Same as the other RegisterType() method, but uses an initializer list
/// instead of an array.
/// They are functionally identical, use whichever you like.
void Entity::RegisterType(name_t name, entity_make constr_func, entity_clear destr_func, std::initializer_list<FieldInfo> fields) {
    FieldInfo* field_copy = nullptr;
    
    if (fields.size()) {
        field_copy = new FieldInfo[fields.size()];
        
        for (auto& field : fields) {
            field_copy[field.field_id] = field;
        }
    }
    
    CheckEntityTypeDuplicate(name, field_copy, fields.size());
    
    registered_entity_types.Insert(name, {constr_func, destr_func, field_copy, fields.size()});
}

/// Registers the entity.
/// Inserts the entity into the entity ID list, so that it can be found using
/// the Entity::Find(id_t) static method.
/// If the entity has a name, then it is also inserted into the name list, so
/// that it can be found using the Entity::Find(name_t) static method.
void Entity::Register() {
    if (id) {
        entity_id_list.Insert(id, this);
    }

    if (name) {
        entity_name_list.Insert(name, this);
    }
}

/// Unregisters the entity.
/// Essentially, this is the reversal of what the Entity::Register() method
/// has done. This method will remove the entity from the ID list and from the
/// name list, if it has a name. It will no longer be possible to find the
/// entity by using the Entity::Find() method and its overloads.
void Entity::Unregister() {
    if (id && entity_id_list.Find(id)) {
        entity_id_list.Remove(id);
    }

    if (name && entity_name_list.Find(name)) {
        entity_name_list.Remove(name);
    }
}

/// Finds the entity by its ID number.
/// @return If an entity with the given ID number has been found, a pointer to
///         it will be returned. Otherwise a `nullptr` will be returned.
Entity* Entity::Find(id_t entityID) {
    return entity_id_list.Find(entityID);
}

/// Finds the entity by its name.
/// @return If an entity with the given name has been found, a pointer to it
///         will be returned. Otherwise a `nullptr` will be returned.
Entity* Entity::Find(name_t entityName) {
    return entity_name_list.Find(entityName);
}

// here we keep the entities that have been unregistered and unloaded, but
// haven't been destructed yet, aka Yeet()'ed entities
static std::vector<Entity*> yeetery;

/// Updates entities.
/// Should be called once per update cycle.
/// This method will go through the list of all yeeted entities and will
/// destruct the ones that need to be destructed.
void Entity::Update() {
    if (!yeetery.size()) return;
    
    // I don't remember why we are making a copy of the yeetery
    auto safe_yeetery = yeetery;
    
    yeetery.clear();
    
    for (auto ent : safe_yeetery) {
        if (ent->IsLoadedFromDisk()) continue;
        
        auto type_info = registered_entity_types.Find(ent->GetType());
        
        if (type_info.destructor) {
            type_info.destructor(ent);
        } else {
            Log(Severity::WARNING, System::CORE, "No destructor for {} typed {} {} found, using default", ent->GetType(), ent->GetName(), ent->GetID());
            delete ent;
        }
    }
}

/// Purges the entity.
/// The entity will be flagged as deleted, unloaded, unregistered and then it
/// will be held in internment until Entity::Update() is called and then, if
/// needed, it will also be destructed.
void Entity::Yeet() {
    yeetery.push_back(this);
    
    this->flags |= DELETED;
    
    if (IsLoaded()) Unload();
    
    Unregister();
}

/// Loads an Entity from a File.
Entity* Entity::Make(name_t type, File* file) {
    auto record = registered_entity_types.Find(type);
    
    if (!record.constructor) return nullptr;
    
    SharedEntityData shared_data {
        type,
        file->read_uint64(),
        file->read_name(),
        file->read_uint32(),
        vec3 {file->read_float32(), file->read_float32(), file->read_float32()},
        vec3 {file->read_float32(), file->read_float32(), file->read_float32()}
    };
    
    static std::vector<Value> fields;
    fields.clear();
    
    for (size_t i = 0; i < record.fieldcount; i++) {
        switch (record.fields[i].field_type) {
            case TYPE_BOOL:     fields.push_back((bool) file->read_uint32()); break; // hehe
            case TYPE_NAME:     fields.push_back(file->read_name());    break;
            case TYPE_STRING:   fields.push_back(nullptr);              break; // TODO: fix
            case TYPE_INT8:     fields.push_back(file->read_int8());    break;
            case TYPE_INT16:    fields.push_back(file->read_int16());   break;
            case TYPE_INT32:    fields.push_back(file->read_int32());   break;
            case TYPE_UINT8:    fields.push_back(file->read_uint8());   break;
            case TYPE_UINT16:   fields.push_back(file->read_uint16());  break;
            case TYPE_UINT32:   fields.push_back(file->read_uint32());  break;
            case TYPE_FLOAT32:  fields.push_back(file->read_float32()); break;
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

/// Creates an entity.
/// Uses data provided in a Entity::RegisterType() call to find the
/// construction function of an entity type and then constructs it.
/// @param  type        Type of the entity, this should be the same name that was
///                     used when using Register() to register an entity type.
/// @param  shared_data Shared data of the entity.
/// @param  field_array Values to be filled in the entity's fields.
/// @return Pointer to an entity if successful, otherwise a `nullptr`.
Entity* Entity::Make(name_t type, const SharedEntityData& shared_data, const ValueArray& field_array) {
    auto record = registered_entity_types.Find(type);
    
    if (!record.constructor) return nullptr;
    
    return record.constructor(shared_data, field_array);
}

}