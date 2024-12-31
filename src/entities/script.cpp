// Tramway Drifting and Dungeon Exploration Simulator SDK Runtime

#include <entities/script.h>

#include <framework/script.h>
#include <framework/worldcell.h>

#include <components/physics.h>

#include <templates/hashmap.h>

#include <config.h>

/**
 * @class tram::Scriptable entities/script.h <entities/script.h>
 * 
 * Provides script language subclassing.
 * 
 * If you want to create a sublass in C++, it's easy -- just `#include` the
 * class header and make subclass. But if you want to do so from a scripting
 * language, e.g. Lua, you will need an interface on the C++ side.
 * This template provides such an interface.
 * 
 * @see https://racenis.github.io/tram-sdk/documentation/entities/script.html
 */

namespace tram {

namespace ScriptableType {

struct ScriptableTypeInfo {
    name_t base_type;
    Entity* (*constr_func)(name_t new_type, const SharedEntityData&, const ValueArray&) = nullptr;
    void (*destr_func)(Entity*) = nullptr;
};

static Hashmap<ScriptableTypeInfo> infos("Scriptable entity type pool", SCRIPTABLE_ENTITY_TYPE_LIMIT);

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