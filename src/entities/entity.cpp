// TRAMWAY DRIFT AND DUNGEON EXPLORATION SIMULATOR 2022
// All rights reserved.
//
// ENTITY.CPP -- Entity base class methods
// Methods that all of the entity types share.

#include <core.h>
#include <serializeddata.h>

namespace Core {
    
    std::unordered_map<uint64_t, Entity*> Entity::List;
    std::unordered_map<name_t, Entity*> Entity::NameList;
    std::unordered_map<std::string, Entity* (*)(std::string_view& params)> Entity::entityConstructors;
    
    void Entity::SerializeString(std::string& str) {
        serialized_data->ToString(str);
    }

    void Entity::Yeet(){
        if(cell) cell->RemoveEntity(this);
        if(isloaded) Unload();
        if(serialized_data) RemoveSerializedData();
        Deregister();
    };

    void Entity::RemoveSerializedData(){
        if(!serialized_data) return;

        delete serialized_data;
        serialized_data = nullptr;
    }
    
    void Entity::CheckTransition(){
        #ifndef ENGINE_EDITOR_MODE
        if(!cell) return;

        WorldCell* into = cell->FindTransition(location);


        if (into) {
            std::cout << ReverseUID(name) << " transitioned into " << ReverseUID(into->GetName()) << std::endl;
            cell->RemoveEntity(this);
            into->AddEntity(this);
        }
        #endif // ENGINE_EDITOR_MODE
    }


    void Entity::Register(const char* name, Entity* (*constr_func)(std::string_view& params)){
        entityConstructors[name] = constr_func;
    }

    void Entity::Register(){
        if (id) {
            Entity::List[id] = this;
        }

        if (name) {
            Entity::NameList[name] = this;
        }
    }

    void Entity::Deregister(){
        if (id) {
            auto id_it = Entity::List.find(id);
            if (id_it != Entity::List.end())
                Entity::List.erase(id_it);
        }

        if (name) {
            auto name_it = Entity::NameList.find(name);
            if (name_it != Entity::NameList.end())
                Entity::NameList.erase(name_it);
        }
    }
    
    Entity* Entity::Find (uint64_t entityID){
        std::unordered_map<uint64_t, Entity*>::iterator ff = Entity::List.find(entityID);

        if(ff == Entity::List.end()){
            return nullptr;
        } else {
            return ff->second;
        }
    }

    Entity* Entity::FindName(name_t entityName){
        std::unordered_map<uint64_t, Entity*>::iterator ff = Entity::NameList.find(entityName);

        if(ff == Entity::NameList.end()){
            return nullptr;
        } else {
            return ff->second;
        }
    }
    
    Entity* MakeEntityFromString(std::string_view& entStr){
        return nullptr; //TODO: find where this function went
    }

    Entity* Entity::Make (std::string_view& params){
        std::cout << params << std::endl;
        size_t st_f = params.find_first_not_of(" \t");
        size_t st_e = params.find_first_of(" \t", st_f);

        std::string entType(params.substr(st_f, st_e - st_f));

        params.remove_prefix(st_e);

        Entity* entit = nullptr;

        std::unordered_map<std::string, Entity* (*)(std::string_view& params)>::iterator ff = entityConstructors.find(entType);
        if (ff != entityConstructors.end()){
            entit = ff->second(params);
            return entit;
        } else {
            return nullptr;
        }


    }

    void Entity::SetParameters (std::string_view& params){
        name = SerializedEntityData::Field<name_t>().FromStringAsName(params);

        location.x = SerializedEntityData::Field<float>().FromString(params);
        location.y = SerializedEntityData::Field<float>().FromString(params);
        location.z = SerializedEntityData::Field<float>().FromString(params);

        float rx = SerializedEntityData::Field<float>().FromString(params);
        float ry = SerializedEntityData::Field<float>().FromString(params);
        float rz = SerializedEntityData::Field<float>().FromString(params);

        rotation = glm::quat(glm::vec3(rx, ry, rz));

        action = SerializedEntityData::Field<name_t>().FromStringAsName(params);
    }

    void Entity::ParametersString(std::string& str){
        // TODO: convert this to new way
        str.append(ReverseUID(name));
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
        str.append(ReverseUID(action));
        str.push_back(' ');
    }

    
    
    
    
}