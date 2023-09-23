// TRAMWAY DRIFT AND DUNGEON EXPLORATION SIMULATOR 2022
// All rights reserved.

#ifndef TRAM_SDK_PHYSICS_COLLISIONMODEL_H
#define TRAM_SDK_PHYSICS_COLLISIONMODEL_H

#include <framework/resource.h>
#include <unordered_map>

class btCompoundShape;

namespace tram::Physics {

class CollisionModel : public Resource {
public:
    CollisionModel(name_t mName) : Resource(mName) {}

    void LoadFromDisk ();
    void LoadFromMemory () {}
    
    void Unload() {}
    
    inline btCompoundShape* GetShape () { return model; }

    static CollisionModel* Find (name_t model_name);
protected:
    btCompoundShape* model = nullptr;
};

}

#endif // TRAM_SDK_PHYSICS_COLLISIONMODEL_H
