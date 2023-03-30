// TRAMWAY DRIFT AND DUNGEON EXPLORATION SIMULATOR 2022
// All rights reserved.

#ifndef FRAMEWORK_ENTITY_H
#define FRAMEWORK_ENTITY_H

#include <framework/uid.h>
#include <framework/core.h>
#include <framework/math.h>

namespace tram {
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
        inline bool IsLoaded() const { return is_loaded; }
        inline bool IsAutoLoad() const { return auto_load; }
        inline bool IsInInterior() { return in_interior; }
        inline bool IsPersistent() { return is_persistent; }
        inline bool IsChanged() { return changed; }

        void ParametersString(std::string& str);

        void virtual UpdateParameters() = 0;
        void virtual SetParameters() = 0;
        
        inline void SetPersistent(bool persistent) { this->is_persistent = persistent; }

        void SetLocation(const vec3& loc) { location = loc; SetParameters(); CheckTransition();}

        void SetRotation(const quat& rot) { rotation = rot; SetParameters(); }

        inline void UpdateTransform(const vec3& loc, const quat& rot){
            location = loc;
            rotation = rot;
            UpdateParameters();
            CheckTransition();
        }

        inline void Move(const glm::vec3& move_by){
            SetLocation(location + move_by);
        }

        // idk why these methods exist
        inline void GetLocation(vec3& loc) { loc = location; }
        inline void GetRotation(glm::quat& rot) { rot = rotation; }
        
        inline const vec3& GetLocation() { return location; }
        inline const quat& GetRotation() { return rotation; }

        virtual void MessageHandler(Message& msg) = 0;

        void CheckTransition();

        static Entity* Make (std::string_view& params);

        static void Register (const char* name, Entity* (*constr_func)(std::string_view& params));

        static Entity* Find (uint64_t entityID);

        static Entity* Find (name_t entityName);
    protected:
        uint64_t id = 0;
        name_t name;
        name_t action;

        bool is_persistent = true;
        bool is_loaded = false;
        bool auto_load = true;
        bool in_interior = false;
        
        bool changed = false;
        
        WorldCell* cell = nullptr;
        
        quat rotation = {0.0f, 0.0f, 0.0f, 1.0f};
        vec3 location = {0.0f, 0.0f, 0.0f};

        void Register();

        friend class WorldCell;
    };
}

#endif // FRAMEWORK_ENTITY_H