// TRAMWAY DRIFT AND DUNGEON EXPLORATION SIMULATOR 2022
// All rights reserved.

#ifndef ENTITY_H
#define ENTITY_H

#include <framework/uid.h>
#include <framework/core.h>


namespace Core {
    class WorldCell;
    class Message;
    
    class Entity {
    public:
        virtual void Load() = 0;

        virtual void Unload() = 0;

        virtual void Serialize() = 0;

        Entity();
        Entity(name_t name);
        Entity(std::string_view& str);
        
        virtual ~Entity();
        
        inline name_t GetName() const { return name; }
        inline uint64_t GetID() const { return id; }
        inline WorldCell* GetCell() { return cell; }
        inline bool IsLoaded() const { return isloaded; }
        inline bool IsAutoLoad() const { return auto_load; }
        inline bool IsInInterior() { return in_interior; }
        inline bool IsPersistent() { return  persistent; }
        inline bool IsChanged() { return changed; }

        void ParametersString(std::string& str);

        void virtual UpdateParameters() = 0;
        void virtual SetParameters() = 0;
        
        inline void SetPersistent(bool persistent) { this->persistent = persistent; }

        void SetLocation(float x, float y, float z){ location = glm::vec3(x, y, z); SetParameters(); CheckTransition(); }
        void SetLocation(const glm::vec3& loc) { location = loc; SetParameters(); CheckTransition();}

        void SetRotation(float x, float y, float z){ rotation = glm::quat(glm::vec3(x, y, z)); SetParameters(); }
        void SetRotation(glm::quat& rot){ rotation = rot; SetParameters(); }

        inline void UpdateTransform(const glm::vec3& loc, const glm::quat& rot){
            location = loc;
            rotation = rot;
            UpdateParameters();
            CheckTransition();
        }

        inline void Move(const glm::vec3& move_by){
            SetLocation(location + move_by);
        }

        inline void GetLocation(glm::vec3& loc) { loc = location; }
        inline const glm::vec3& GetLocation() { return location; }

        inline void GetRotation(glm::quat& rot) { rot = rotation; }
        inline const glm::quat& GetRotation() { return rotation; }

        virtual void MessageHandler(Message& msg) = 0;

        void CheckTransition();

        static Entity* Make (std::string_view& params);

        static void Register (const char* name, Entity* (*constr_func)(std::string_view& params));

        // TODO: rename this to FindByID
        static Entity* Find (uint64_t entityID);

        // TODO: rename this to FindByName
        static Entity* FindName(name_t entityName);
    protected:
        uint64_t id = 0;
        name_t name = 0;
        name_t action = 0;

        bool isloaded = false;
        bool auto_load = true;
        bool in_interior = false;
        bool persistent = true;
        bool changed = false;
        
        WorldCell* cell = nullptr;
        
        glm::quat rotation = {0.0f, 0.0f, 0.0f, 1.0f};
        glm::vec3 location = {0.0f, 0.0f, 0.0f};

        void Register();

        friend class WorldCell;
    };
}

#endif // ENTITY_H