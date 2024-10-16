// TRAMWAY DRIFT AND DUNGEON EXPLORATION SIMULATOR 2022
// All rights reserved.

#ifndef TRAM_SDK_FRAMEWORK_ENTITY_H
#define TRAM_SDK_FRAMEWORK_ENTITY_H

#include <framework/uid.h>
#include <framework/core.h>
#include <framework/math.h>
#include <framework/value.h>
#include <framework/signal.h>

namespace tram {
class WorldCell;
class Message;
struct SharedEntityData;
struct ValueArray;
class File;

class Entity {
public:
    virtual void Load() = 0;

    virtual void Unload() = 0;

    virtual void Serialize() = 0;

    Entity();
    Entity(name_t name);
    Entity(const SharedEntityData&);
    
    virtual ~Entity();
    
    inline name_t GetName() const { return name; }
    inline id_t GetID() const { return id; }
    inline WorldCell* GetCell() { return cell; }
    inline bool IsLoaded() const { return is_loaded; }
    inline bool IsAutoLoad() const { return auto_load; }
    inline bool IsInInterior() const { return in_interior; }
    inline bool IsPersistent() const { return is_persistent; }
    inline bool IsChanged() const { return changed; }

    void virtual UpdateParameters() = 0;
    void virtual SetParameters() = 0;
    
    void virtual Update() {}
    
    inline void SetAutoLoad(bool auto_load) { this->auto_load = auto_load; }
    inline void SetPersistent(bool persistent) { this->is_persistent = persistent; }

    void SetLocation(vec3 loc) { location = loc; SetParameters(); CheckTransition();}
    void SetRotation(quat rot) { rotation = rot; SetParameters(); }

    inline void UpdateTransform(const vec3& loc, const quat& rot){
        location = loc;
        rotation = rot;
        UpdateParameters();
        CheckTransition();
    }
    
    inline const vec3& GetLocation() { return location; }
    inline const quat& GetRotation() { return rotation; }

    virtual void MessageHandler(Message& msg) = 0;
    virtual void EventHandler(Event &event) {};

    virtual name_t GetType() = 0;

    void CheckTransition();

    struct FieldInfo {
        uint32_t field_id;
        uint32_t field_type;
        uint32_t field_flags;
    };

    enum : uint32_t {
        FIELD_SERIALIZE = 1
    };

    static void RegisterType(name_t name, Entity* (*constr_func)(const SharedEntityData&, const ValueArray&), void (*destr_func)(Entity*), const uint32_t* fields, size_t fieldcount);
    static void RegisterType(name_t name, Entity* (*constr_func)(const SharedEntityData&, const ValueArray&), void (*destr_func)(Entity*), std::initializer_list<FieldInfo> fields);
    
    inline SignalTable* GetSignalTable() { return signals; }
    
    static void UpdateFromList();
    static Entity* Make (name_t type, File* file);
    static Entity* Find (id_t entity_id);
    static Entity* Find (name_t entity_name);
protected:
    id_t id = 0;
    name_t name;
    
    uint32_t flags = 0;

    bool is_persistent = true;
    bool is_loaded = false;
    bool auto_load = true;
    bool in_interior = false;
    
    bool changed = false;
    bool is_serializable = true;
    
    WorldCell* cell = nullptr;
    SignalTable* signals = nullptr;
    
    quat rotation = {1.0f, 0.0f, 0.0f, 0.0f};
    vec3 location = {0.0f, 0.0f, 0.0f};

    void Register();
    inline void FireSignal(signal_t type) { if (signals) signals->Fire(type, this->id); }
    inline void FireSignal(signal_t type, Value value) { if (signals) signals->Fire(type, this->id, value); }
    void AddUpdate();
    void RemoveUpdate();

    friend class WorldCell;
};

}

#endif // TRAM_SDK_FRAMEWORK_ENTITY_H