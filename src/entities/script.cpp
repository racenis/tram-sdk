// TRAMWAY DRIFT AND DUNGEON EXPLORATION SIMULATOR 2022
// All rights reserved.

#include <entities/script.h>

#include <framework/script.h>
#include <framework/worldcell.h>

#include <components/physics.h>

#include <templates/hashmap.h>

namespace tram {

namespace ScriptableType {

struct ScriptableTypeInfo {
    name_t base_type;
    Entity* (*constr_func)(name_t new_type, const SharedEntityData&, const ValueArray&) = nullptr;
    void (*destr_func)(Entity*) = nullptr;
};

static Hashmap<ScriptableTypeInfo> infos("scruiptable uinfos", 100);

void Register(name_t base_type,  Entity* (*constr_func)(name_t new_type, const SharedEntityData&, const ValueArray&), void (*destr_func)(Entity*)) {
    infos.Insert(base_type, {base_type, constr_func, destr_func});
}

Entity* Make(name_t base_type, name_t new_type, const SharedEntityData& data, const ValueArray& array) {
    const auto& info = infos.Find(base_type);
    
    if (!info.constr_func) return nullptr;
    
    return info.constr_func(new_type, data, array);
}

void Yeet(Entity* yeetable) {
    const auto& info = infos.Find(yeetable->GetType());
    
    assert(info.destr_func);
    
    return info.destr_func(yeetable);
}

}

}