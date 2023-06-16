// TRAMWAY DRIFT AND DUNGEON EXPLORATION SIMULATOR 2022
// All rights reserved.

#include <physics/physics.h>
#include <physics/bullet/bullet.h>
#include <physics/bullet/actions.h>


#include <components/trigger.h>


#include <framework/system.h>

namespace tram::Physics {
/// Option for drawing physics debug lines.
/// Set to true if you want to see physics debug lines. False if not.
bool DRAW_PHYSICS_DEBUG = false;

/// Initializes the physics system.
void Init() {
    assert(!System::IsInitialized(System::SYSTEM_PHYSICS));
    
    Bullet::Init();
    
    System::SetInitialized(System::SYSTEM_PHYSICS, true);
}

/// Updates the physics system.
/// Should only be called once in the update cycle.
void Update() {
    Bullet::StepPhysics();
    
    if (DRAW_PHYSICS_DEBUG) Bullet::DrawDebug();
}

/// Performs a raycast.
/// Performs a raycast from from to to.
/// @return Collision struct. If there was nothing found, then the pointer
///         in the struct will be set to nullptr.
Collision Raycast(const glm::vec3& from, const glm::vec3& to, uint32_t collision_mask) {
    return Bullet::Raycast(from, to, collision_mask);
}

/// I have no idea if this function works.
std::vector<Collision> Shapecast(const CollisionShape& shape, const vec3& from, const vec3& to, uint32_t collision_mask) {
    return Bullet::Shapecast(shape, from, to, collision_mask);
}

}
