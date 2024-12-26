// Tramway Drifting and Dungeon Exploration Simulator SDK Runtime

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
    inline bool IsLoaded() const { return flags & LOADED; }
    inline bool IsAutoLoad() const { return !(flags & DISABLE_AUTO_LOAD); }
    inline bool IsPersistent() const { return !(flags & NON_PERSISTENT); }
    inline bool IsChanged() const { return flags & DIRTY; }
    inline bool IsDeleted() const { return flags & DELETED; }
    inline bool IsLoadedFromDisk() const { return flags & LOADED_FROM_DISK; }

    void virtual UpdateParameters() = 0;
    void virtual SetParameters() = 0;
    
    static void Update();
    void Yeet();
    
    inline void SetAutoLoad(bool is) { flags = is ? flags & ~DISABLE_AUTO_LOAD : flags | DISABLE_AUTO_LOAD; }
    inline void SetPersistent(bool is) { flags = is ? flags & ~NON_PERSISTENT : flags | NON_PERSISTENT; }

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

    // TODO convert constr_func and destr_func into typedefs
    static void RegisterType(name_t name, Entity* (*constr_func)(const SharedEntityData&, const ValueArray&), void (*destr_func)(Entity*), const uint32_t* fields, size_t fieldcount);
    static void RegisterType(name_t name, Entity* (*constr_func)(const SharedEntityData&, const ValueArray&), void (*destr_func)(Entity*), const FieldInfo* fields, size_t fieldcount);
    static void RegisterType(name_t name, Entity* (*constr_func)(const SharedEntityData&, const ValueArray&), void (*destr_func)(Entity*), std::initializer_list<FieldInfo> fields);
    
    inline SignalTable* GetSignalTable() { return signals; }
    
    static void UpdateFromList();
    static Entity* Make(name_t type, File* file);
    static Entity* Make(name_t type, const SharedEntityData&, const ValueArray&);
    static Entity* Find(id_t entity_id);
    static Entity* Find(name_t entity_name);
protected:
    enum : uint32_t {
        NON_PERSISTENT = 1,
        LOADED = 2,
        DISABLE_AUTO_LOAD = 4,
        NON_SERIALIZABLE = 8,
        DIRTY = 16,
        DELETED = 32,
        LOADED_FROM_DISK = 64
    };

    id_t id = 0;
    name_t name;
    
    WorldCell* cell = nullptr;
    
    // don't really need this, since we can use the entity ID number to lookup
    // the SignalTable for each entity
    // TODO: fix
    SignalTable* signals = nullptr;
    
    quat rotation = {1.0f, 0.0f, 0.0f, 0.0f};
    vec3 location = {0.0f, 0.0f, 0.0f};

    uint32_t flags = 0;

    void Register();
    void Unregister();
    inline void FireSignal(signal_t type) { if (signals) signals->Fire(type, this->id); }
    inline void FireSignal(signal_t type, Value value) { if (signals) signals->Fire(type, this->id, value); }

    friend class WorldCell;
};

struct SharedEntityData {
    name_t type;
    uint64_t id = 0;
    name_t name;
    uint32_t flags = 0;
    vec3 position = {0.0f, 0.0f, 0.0f};
    quat rotation = {1.0f, 0.0f, 0.0f, 0.0f};;
};

}

#endif // TRAM_SDK_FRAMEWORK_ENTITY_H