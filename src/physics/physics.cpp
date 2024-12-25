// Tramway Drifting and Dungeon Exploration Simulator SDK Runtime


#include <physics/physics.h>
#include <physics/api.h>

#include <framework/system.h>
#include <framework/stats.h>

namespace tram::Physics {

/// Initializes the physics system.
void Init() {
    assert(!System::IsInitialized(System::SYSTEM_PHYSICS));
    
    API::Init();
    
    System::SetInitialized(System::SYSTEM_PHYSICS, true);
}

/// Updates the physics system.
/// Should only be called once in the update cycle.
void Update() {
    Stats::Start(System::SYSTEM_PHYSICS);
    API::StepPhysics();
    Stats::Stop(System::SYSTEM_PHYSICS);
}

/// Performs a raycast.
/// Performs a raycast from from to to.
/// @return Collision struct. If there was nothing found, then the pointer
///         in the struct will be set to nullptr.
Collision Raycast(const vec3& from, const vec3& to, uint32_t collision_mask) {
    auto[result, object] = API::Raycast(from, to, collision_mask);
    
    return {(PhysicsComponent*)object, result.point, result.normal};
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
