// Tramway Drifting and Dungeon Exploration Simulator SDK Runtime


#ifndef TRAM_SDK_PHYSICS_COLLISIONMODEL_H
#define TRAM_SDK_PHYSICS_COLLISIONMODEL_H

#include <framework/resource.h>
#include <physics/physics.h>

namespace tram::Physics {

class CollisionModel : public Resource {
public:
    CollisionModel(name_t mName) : Resource(mName) {}

    void LoadFromDisk();
    void LoadFromMemory() {}
    void Unload() {}
    
    inline collisionshape_t GetShape() { return shape; }

    static CollisionModel* Find(name_t model_name);
protected:
    collisionshape_t shape;
};

}

#endif // TRAM_SDK_PHYSICS_COLLISIONMODEL_H
