// Tramway Drifting and Dungeon Exploration Simulator SDK Runtime

#ifndef TRAM_SDK_PHYSICS_COLLISIONSHAPE_H
#define TRAM_SDK_PHYSICS_COLLISIONSHAPE_H

#include <framework/math.h>

/**
 * @struct tram::Physics::CollisionShape physics/collisionshape.h <physics/collisionshape.h>
 * 
 * Shape for a collider.
 * 
 * @see https://racenis.github.io/tram-sdk/documentation/physics/collisionshape.html
 */

namespace tram::Physics {
    
enum Shape : uint32_t {
    SHAPE_SPHERE,
    SHAPE_CYLINDER,
    SHAPE_CAPSULE,
    SHAPE_CONE,
    SHAPE_BOX,
    SHAPE_HULL,
    SHAPE_MESH
};

struct CollisionTriangle {
    vec3 p0, p1, p2;
};

struct CollisionShape {
    Shape type;
    
    union {
        struct {
            union {
                float radius;
                float radius_x;
                float extent_x;
            };
            
            union {
                float height;
                float extent_y;
            };
            
            union {
                float extent_z;
                float radius_z;
            };
        };
        
        struct {
            vec3* hull_points;
            size_t hull_size;
        };
        
        struct {
            CollisionTriangle* mesh_triangles;
            size_t mesh_size;
        };
    };
    
    static inline CollisionShape Sphere(float radius);
    static inline CollisionShape Cylinder(float radius, float height);
    static inline CollisionShape Capsule(float radius, float height);
    static inline CollisionShape Cone(float radius, float height);
    static inline CollisionShape Box(vec3 dimensions);
    static inline CollisionShape Hull(vec3* points, size_t size);
    static inline CollisionShape Mesh(CollisionTriangle* triangles, size_t size);
};

/// Creates a sphere.
CollisionShape CollisionShape::Sphere(float radius) {
    return {SHAPE_SPHERE, radius, 0.0f, 0.0f};
}

/// Creates a cylinder.
CollisionShape CollisionShape::Cylinder(float radius, float height) {
    return {SHAPE_CYLINDER, radius, height, radius};
}

/// Creates a capsule.
CollisionShape CollisionShape::Capsule(float radius, float height) {
    return {SHAPE_CAPSULE, radius, height, 0.0f};
}

/// Creates a cone.
CollisionShape CollisionShape::Cone(float radius, float height) {
    return {SHAPE_CONE, radius, height, 0.0f};
}

/// Creates a box.
CollisionShape CollisionShape::Box(vec3 dimensions) {
    return {SHAPE_BOX, dimensions.x, dimensions.y, dimensions.z};
}

/// Creates a convex hull.
/// @note Memory pointed to by the point pointer should not be freed until the
///       CollisionShape is no longer in use.
CollisionShape CollisionShape::Hull(vec3* points, size_t size) {
    return CollisionShape {.type = SHAPE_HULL, .hull_points = points, .hull_size = size};
}

/// Creates a triangle mesh.
/// @note Memory pointed to by the triangle pointer should not be freed until
///       the CollisionShape is no longer in use.
CollisionShape CollisionShape::Mesh(CollisionTriangle* triangles, size_t size) {
    return CollisionShape {.type = SHAPE_MESH, .mesh_triangles = triangles, .mesh_size = size};
}

}

#endif // TRAM_SDK_PHYSICS_COLLISIONSHAPE_H