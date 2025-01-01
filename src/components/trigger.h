// Tramway Drifting and Dungeon Exploration Simulator SDK Runtime

#ifndef TRAM_SDK_COMPONENTS_TRIGGERCOMPONENT_H
#define TRAM_SDK_COMPONENTS_TRIGGERCOMPONENT_H

#include <physics/physics.h>
#include <components/physics.h>

#include <vector>

namespace tram {

class TriggerComponent;

typedef void (*collision_callback)(TriggerComponent*, Physics::Collision);
typedef bool (*filter_callback)(TriggerComponent*, PhysicsComponent*);
    
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

    void SetActivationCallback(collision_callback activation) { this->activation = activation; }
    void SetDectivationCallback(collision_callback deactivation) { this->deactivation = deactivation; }
    void SetFilterCallback(filter_callback filter) { this->filter = filter; }

    void SetCollisionMask(uint32_t flags);
    void SetCollisionGroup(uint32_t flags);
    
    void SetStoreCollisions(bool store_collisions) { this->store_collisions = store_collisions; }
    
    uint32_t GetCollisionMask() { return collisionMask; }
    uint32_t GetCollisionGroup() { return collisionGroup; }
    
    void SetLocation(vec3 location);
    void SetRotation(quat rotation);
    
    const std::vector<Physics::Collision>& GetStoredCollisions() { return stored_collisions; }
    
    // why are these not private/protected?
    // TODO: fix
    void Collision(const Physics::Collision& collision);
    void ResetCollisions();
    
    std::vector<Physics::Collision> Poll();
private:
    ResourceProxy<Physics::CollisionModel> model;
    
    // TODO: why camelCase?
    uint32_t collisionMask = -1;
    uint32_t collisionGroup = -1;
    
    // TODO: change to {} initialization?
    vec3 location = vec3(0.0f, 0.0f, 0.0f);
    quat rotation = vec3(0.0f, 0.0f, 0.0f);
    
    Physics::collisionshape_t shape = {nullptr};
    Physics::trigger_t trigger = {nullptr};
    
    collision_callback activation = nullptr;
    collision_callback deactivation = nullptr;
    filter_callback filter = nullptr;

    bool store_collisions = false;
    
    std::vector<std::pair<uint32_t, Physics::Collision>> active_collisions;
    
    std::vector<Physics::Collision> stored_collisions;
};

}

#endif // TRAM_SDK_COMPONENTS_TRIGGERCOMPONENT_H