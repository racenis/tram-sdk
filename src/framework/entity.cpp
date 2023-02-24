// TRAMWAY DRIFT AND DUNGEON EXPLORATION SIMULATOR 2022
// All rights reserved.

#include <framework/entity.h>
#include <framework/worldcell.h>
#include <framework/etc.h>
#include <framework/serializeddata.h>

#include <templates/hashmap.h>

#include <iostream>
#include <unordered_map>

namespace Core {
    // TODO: swap all of these out for my own hashmap
    //std::unordered_map<uint64_t, Entity*> ENTITY_ID_LIST;
    Hashmap<Entity*> ENTITY_ID_LIST ("Entity ID hashmap", 500);
    //std::unordered_map<uint64_t, Entity*> ENTITY_NAME_LIST;
    Hashmap<Entity*> ENTITY_NAME_LIST ("Entity name hashmap", 500);
    std::unordered_map<std::string, Entity* (*)(std::string_view& params)> ENTITY_CONSTRUCTORS; //  !!!!
    
    Entity::Entity() {
        this->id = GenerateID();
        Register();
    }
    
    Entity::Entity(name_t name) {
        this->id = GenerateID();
        this->name = name;
        Register();
    }
    
    Entity::Entity(std::string_view& str) {
        name = SerializedEntityData::Field<name_t>().FromString(str);

        location.x = SerializedEntityData::Field<float>().FromString(str);
        location.y = SerializedEntityData::Field<float>().FromString(str);
        location.z = SerializedEntityData::Field<float>().FromString(str);

        float rx = SerializedEntityData::Field<float>().FromString(str);
        float ry = SerializedEntityData::Field<float>().FromString(str);
        float rz = SerializedEntityData::Field<float>().FromString(str);

        rotation = glm::quat(glm::vec3(rx, ry, rz));

        action = SerializedEntityData::Field<name_t>().FromString(str);
        
        if (!id) id = GenerateID();
        Register();
    }
    
    Entity::~Entity() {
        if (cell) cell->RemoveEntity(this);

        if (id) {
            //auto id_it = ENTITY_ID_LIST.find(id);
            //if (id_it != ENTITY_ID_LIST.end()) ENTITY_ID_LIST.erase(id_it);
            if (ENTITY_ID_LIST.Find(id)) {
                ENTITY_ID_LIST.Insert(id, nullptr);
            }
        }

        if (name) {
            //auto name_it = ENTITY_NAME_LIST.find(name.key);
            //if (name_it != ENTITY_NAME_LIST.end()) ENTITY_NAME_LIST.erase(name_it);
            if (ENTITY_NAME_LIST.Find(name)) {
                ENTITY_NAME_LIST.Insert(name, nullptr);
            }
        }
    }
    
    void Entity::CheckTransition(){
        #ifndef ENGINE_EDITOR_MODE
        if(!cell) return;

        WorldCell* into = cell->FindTransition(location);


        if (into) {
            std::cout << name << " transitioned into " << into->GetName() << std::endl;
            cell->RemoveEntity(this);
            into->AddEntity(this);
        }
        #endif // ENGINE_EDITOR_MODE
    }

    void Entity::Register(const char* name, Entity* (*constr_func)(std::string_view& params)){
        ENTITY_CONSTRUCTORS[name] = constr_func;
    }

    void Entity::Register(){
        if (id) {
            //ENTITY_ID_LIST[id] = this;
            ENTITY_ID_LIST.Insert(id, this);
        }

        if (name) {
            //ENTITY_NAME_LIST[name.key] = this;
            ENTITY_NAME_LIST.Insert(name, this);
        }
    }
    
    Entity* Entity::FindByID (uint64_t entityID){
        /*std::unordered_map<uint64_t, Entity*>::iterator ff = ENTITY_ID_LIST.find(entityID);

        if(ff == ENTITY_ID_LIST.end()){
            return nullptr;
        } else {
            return ff->second;
        }*/
        
        return ENTITY_ID_LIST.Find(entityID);
    }

    Entity* Entity::FindByName(name_t entityName){
        /*std::unordered_map<uint64_t, Entity*>::iterator ff = ENTITY_NAME_LIST.find(entityName.key);

        if(ff == ENTITY_NAME_LIST.end()){
            return nullptr;
        } else {
            return ff->second;
        }*/
        
        return ENTITY_NAME_LIST.Find(entityName);
    }
    
    Entity* MakeEntityFromString(std::string_view& entStr){
        return nullptr; //TODO: find where this function went
    }

    Entity* Entity::Make (std::string_view& params){
        size_t st_f = params.find_first_not_of(" \t");
        size_t st_e = params.find_first_of(" \t", st_f);

        std::string entType(params.substr(st_f, st_e - st_f));

        params.remove_prefix(st_e);

        Entity* entit = nullptr;

        std::unordered_map<std::string, Entity* (*)(std::string_view& params)>::iterator ff = ENTITY_CONSTRUCTORS.find(entType);
        if (ff != ENTITY_CONSTRUCTORS.end()){
            entit = ff->second(params);
            return entit;
        } else {
            return nullptr;
        }


    }

    void Entity::ParametersString(std::string& str){
        // TODO: convert this to new way
        str.append(std::string(name));
        str.push_back(' ');
        str.append(std::to_string(location.x));
        str.push_back(' ');
        str.append(std::to_string(location.y));
        str.push_back(' ');
        str.append(std::to_string(location.z));
        str.push_back(' ');


        glm::vec3 rot = glm::eulerAngles(rotation);

        str.append(std::to_string(rot.x));
        str.push_back(' ');
        str.append(std::to_string(rot.y));
        str.push_back(' ');
        str.append(std::to_string(rot.z));
        str.push_back(' ');
        str.append(std::string(action));
        str.push_back(' ');
    }
}