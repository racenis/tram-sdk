// TRAMWAY DRIFT AND DUNGEON EXPLORATION SIMULATOR 2022
// All rights reserved.

#ifndef PHYSICS_COLLISIONMODEL_H
#define PHYSICS_COLLISIONMODEL_H

#include <framework/resource.h>
#include <unordered_map>

class btCompoundShape;

namespace Core::Physics {
    class CollisionModel : public Resource {
    public:
        CollisionModel(){}
        CollisionModel(name_t mName){
            name = mName;
            status = UNLOADED;
        }

        void LoadFromDisk();
        void LoadFromMemory(){}
        inline btCompoundShape* GetPtr() {return model;}

        static CollisionModel* Find(name_t modelName);
    protected:
        btCompoundShape* model = nullptr;
    };
}

#endif // PHYSICS_COLLISIONMODEL_H
