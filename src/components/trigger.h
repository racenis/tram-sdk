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
    static TriggerComponent* Make();
    static void Yeet(TriggerComponent* component);
    
    Physics::CollisionModel* GetModel() const { return model.get(); }
    void SetModel(name_t model);
    void SetModel(Physics::CollisionModel* model);
    void SetShape(Physics::CollisionShape shape);

    void SetActivationCallback(collision_callback activation) { this->activation = activation; }
    void SetDectivationCallback(collision_callback deactivation) { this->deactivation = deactivation; }
    void SetFilterCallback(filter_callback filter) { this->filter = filter; }

    void SetCollisionMask(uint32_t flags);
    void SetCollisionGroup(uint32_t flags);
    
    void SetStoreCollisions(bool store_collisions) { this->store_collisions = store_collisions; }
    
    uint32_t GetCollisionMask() const { return collision_mask; }
    uint32_t GetCollisionGroup() const { return collision_group; }
    
    void SetLocation(vec3 location);
    void SetRotation(quat rotation);
    
    const std::vector<Physics::Collision>& GetStoredCollisions() const { return stored_collisions; }

    void EventHandler(Event &event) {}
private:
    ResourceProxy<Physics::CollisionModel> model;
    
    uint32_t collision_mask = -1;
    uint32_t collision_group = -1;
    
    vec3 location = {0.0f, 0.0f, 0.0f};
    quat rotation = {1.0f, 0.0f, 0.0f, 0.0f};
    
    Physics::collisionshape_t shape = {nullptr};
    Physics::trigger_t trigger = {nullptr};
    
    collision_callback activation = nullptr;
    collision_callback deactivation = nullptr;
    filter_callback filter = nullptr;

    bool store_collisions = false;
    
    void Start();
    
    void Collision(const Physics::Collision& collision);
    void ResetCollisions();
    
    TriggerComponent() : model(this){}
    ~TriggerComponent();
    
    friend void Physics::Update();
    
    std::vector<std::pair<uint32_t, Physics::Collision>> active_collisions;
    std::vector<Physics::Collision> stored_collisions;
};

}

#endif // TRAM_SDK_COMPONENTS_TRIGGERCOMPONENT_H