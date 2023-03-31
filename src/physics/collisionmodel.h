// TRAMWAY DRIFT AND DUNGEON EXPLORATION SIMULATOR 2022
// All rights reserved.

#ifndef PHYSICS_COLLISIONMODEL_H
#define PHYSICS_COLLISIONMODEL_H

#include <framework/resource.h>
#include <unordered_map>

class btCompoundShape;

namespace tram::Physics {
    class CollisionModel : public Resource {
    public:
        CollisionModel(name_t mName) : Resource(mName){
            name = mName;
            status = UNLOADED;
        }

        void LoadFromDisk();
        void LoadFromMemory(){}
        inline btCompoundShape* GetShape () { return model; }

        static CollisionModel* Find(name_t modelName);
    protected:
        btCompoundShape* model = nullptr;
    };
}

#endif // PHYSICS_COLLISIONMODEL_H
