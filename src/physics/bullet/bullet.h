// TRAMWAY DRIFT AND DUNGEON EXPLORATION SIMULATOR 2022
// All rights reserved.

#ifndef PHYSICS_BULLET_H
#define PHYSICS_BULLET_H

#include <physics/physics.h>

namespace Core::Physics {
    btConvexShape* CollisionShapeToConvexShape (CollisionShape shape);
}

#endif // PHYSICS_BULLET_H
