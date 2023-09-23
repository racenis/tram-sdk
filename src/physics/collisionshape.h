// TRAMWAY DRIFT AND DUNGEON EXPLORATION SIMULATOR 2022
// All rights reserved.

#ifndef TRAM_SDK_PHYSICS_COLLISIONSHAPE_H
#define TRAM_SDK_PHYSICS_COLLISIONSHAPE_H

#include <framework/math.h>

namespace tram::Physics {
    
enum Shape : uint32_t {
    SHAPE_SPHERE,
    SHAPE_CYLINDER,
    SHAPE_CAPSULE,
    SHAPE_CONE,
    SHAPE_BOX
};

struct CollisionShape {
    Shape type;
    vec3 dimensions;
    
    static inline CollisionShape Sphere (float radius);
    static inline CollisionShape Cylinder (float radius, float height);
    static inline CollisionShape Capsule (float radius, float height);
    static inline CollisionShape Cone (float radius, float height);
    static inline CollisionShape Box (vec3 dimensions);
};

CollisionShape CollisionShape::Sphere (float radius) {
    return {SHAPE_SPHERE, {radius, 0.0f, 0.0f}};
}

CollisionShape CollisionShape::Cylinder (float radius, float height) {
    return {SHAPE_CYLINDER, {radius, height, radius}};
}

CollisionShape CollisionShape::Capsule (float radius, float height) {
    return {SHAPE_CAPSULE, {radius, height, 0.0f}};
}

CollisionShape CollisionShape::Cone (float radius, float height) {
    return {SHAPE_CONE, {radius, height, 0.0f}};
}

CollisionShape CollisionShape::Box (vec3 dimensions) {
    return {SHAPE_BOX, dimensions};
}

}

#endif // TRAM_SDK_PHYSICS_COLLISIONSHAPE_H