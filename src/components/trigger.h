// TRAMWAY DRIFT AND DUNGEON EXPLORATION SIMULATOR 2022
// All rights reserved.

#ifndef TRAM_SDK_COMPONENTS_TRIGGERCOMPONENT_H
#define TRAM_SDK_COMPONENTS_TRIGGERCOMPONENT_H

#include <physics/physics.h>
#include <components/physics.h>

#include <vector>

class btCollisionShape;
class btCollisionObject;

namespace tram {

class TriggerComponent : public EntityComponent {
public:
    TriggerComponent() : model(this){}
    ~TriggerComponent();
    void EventHandler(Event &event){};
    void Start();

    Physics::CollisionModel* GetModel() { return model.get(); }
    void SetModel(name_t model) { this->model = Physics::CollisionModel::Find(model); }
    void SetModel(Physics::CollisionModel* model) { this->model = model; }
    void SetShape(Physics::CollisionShape shape);

    void SetActivationCallback(void (*activation_callback)(TriggerComponent*, Physics::Collision)) { this->activation_callback = activation_callback; }
    void SetDectivationCallback(void (*deactivation_callback)(TriggerComponent*, Physics::Collision)) { this->deactivation_callback = deactivation_callback; }
    void SetFilterCallback(bool (*filter_callback)(TriggerComponent*, PhysicsComponent*)) { this->filter_callback = filter_callback; }

    void SetCollisionMask(uint32_t flags);
    void SetCollisionGroup(uint32_t flags);
    
    void SetStoreCollisions(bool store_collisions) { this->store_collisions = store_collisions; }
    
    uint32_t GetCollisionMask() { return collisionMask; }
    uint32_t GetCollisionGroup() { return collisionGroup; }
    
    void SetLocation(vec3 location);
    void SetRotation(quat rotation);
    
    const std::vector<Physics::Collision>& GetStoredCollisions() { return stored_collisions; }
    
    void Collision(const Physics::Collision& collision);
    void ResetCollisions();
    
    std::vector<Physics::Collision> Poll ();
private:
    ResourceProxy<Physics::CollisionModel> model;
    
    uint32_t collisionMask = -1;
    uint32_t collisionGroup = -1;
    
    vec3 location = vec3(0.0f, 0.0f, 0.0f);
    quat rotation = vec3(0.0f, 0.0f, 0.0f);
    
    Physics::collisionshape_t shape = {nullptr};
    Physics::trigger_t trigger = {nullptr};
    
    void (*activation_callback)(TriggerComponent*, Physics::Collision) = nullptr;
    void (*deactivation_callback)(TriggerComponent*, Physics::Collision) = nullptr;
    bool (*filter_callback)(TriggerComponent*, PhysicsComponent*) = nullptr;

    bool store_collisions = false;
    
    std::vector<std::pair<uint32_t, Physics::Collision>> active_collisions;
    
    std::vector<Physics::Collision> stored_collisions;
};

}

#endif // TRAM_SDK_COMPONENTS_TRIGGERCOMPONENT_H