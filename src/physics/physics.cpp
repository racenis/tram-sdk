// Tramway Drifting and Dungeon Exploration Simulator SDK Runtime

#include <physics/physics.h>
#include <physics/api.h>

#include <framework/system.h>
#include <framework/stats.h>

/**
 * @struct tram::Physics::Collision physics/physics.h <physics/physics.h>
 * 
 * Physics collision data.
 * 
 * Used to represent a collision between two physics objects. Sometimes a
 * function always returns a Collision struct, but it can still fail, such as
 * the Raycast() function, which can perform a raycast that doesn't hit
 * anything. In such a case the structs vector properties will be zeroed-out and
 * the `collider` will be set to a `nullptr`.
 * 
 * @property tram::Physics::Collision::collider
 * Pointer to a PhysicsComponent that owns the rigidbody with which the
 * collision happened.
 * 
 * @property tram::Physics::Collision::point
 * Position of the collision in world space.
 * 
 * @property tram::Physics::Collision::normal
 * Normal vector of the surface that the collision happened with.
 * 
 * @property tram::Physics::Collision::distance
 * Distance of the collision, i.e. how far the colliders penetrated each other.
 */

namespace tram::Physics {

/// Initializes the physics system.
void Init() {
    System::SetState(System::PHYSICS, System::INIT);
    System::AssertDependency(System::CORE);
    
    API::Init();
    
    System::SetState(System::PHYSICS, System::READY);
}

/// Updates the physics system.
/// Should only be called once in the update cycle.
void Update() {
    Stats::Start(System::PHYSICS);
    
    API::StepPhysics();
    
    Stats::Stop(System::PHYSICS);
}

/// Performs a raycast.
/// Performs a raycast from from to to.
/// @return Collision struct. If there was nothing found, then the pointer
///         in the struct will be set to nullptr.
Collision Raycast(const vec3& from, const vec3& to, uint32_t collision_mask) {
    auto[result, object] = API::Raycast(from, to, collision_mask);
    
    return {(PhysicsComponent*)object, result.point, result.normal, result.distance};
}

/// I have no idea if this function works. (yes it does)
std::vector<Collision> Shapecast(const CollisionShape& shape, const vec3& from, const vec3& to, uint32_t collision_mask) {
    std::vector<Collision> collisions;
    auto results = API::Shapecast(shape, from, to, collision_mask);
    for (auto[result, object] : results) {
        collisions.push_back({((PhysicsComponent*) object), result.point, result.normal, result.distance});
    }
    
    return collisions;
}

}
