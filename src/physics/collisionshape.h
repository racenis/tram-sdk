// TRAMWAY DRIFT AND DUNGEON EXPLORATION SIMULATOR 2022
// All rights reserved.

#ifndef PHYSICS_COLLISIONSHAPE_H
#define PHYSICS_COLLISIONSHAPE_H

#include <physics/physics.h>

namespace Core::Physics {
    struct CollisionShape {
        Shape type;
        vec3 dimensions;
        
        static inline CollisionShape Sphere (float radius);
        static inline CollisionShape Cylinder (float radius, float height);
        static inline CollisionShape Capsule (float radius, float height);
        static inline CollisionShape Box (vec3 dimensions);
    };
    
    CollisionShape CollisionShape::Sphere (float radius) {
        return {SHAPE_SPHERE, vec3(radius, 0.0f, 0.0f)};
    }
    
    CollisionShape CollisionShape::Cylinder (float radius, float height) {
        return {SHAPE_CYLINDER, vec3(radius, height, radius)};
    }
    
    CollisionShape CollisionShape::Capsule (float radius, float height) {
        return {SHAPE_CAPSULE, vec3(radius, height, 0.0f)};
    }
    
    CollisionShape CollisionShape::Box (vec3 dimensions) {
        return {SHAPE_BOX, dimensions};
    }
    
}

#endif // PHYSICS_COLLISIONSHAPE_H