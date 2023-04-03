// TRAMWAY DRIFT AND DUNGEON EXPLORATION SIMULATOR 2022
// All rights reserved.

#ifndef COMPONENTS_TRIGGERCOMPONENT_H
#define COMPONENTS_TRIGGERCOMPONENT_H

#include <physics/physics.h>
#include <components/physicscomponent.h>

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

    name_t GetModel(){ return model->GetName(); }
    void SetModel(name_t modelName){ model = Physics::CollisionModel::Find(modelName); }
    void SetShape(Physics::CollisionShape shape);

    void SetActivationCallback(void (*activation_callback)(TriggerComponent*, Physics::Collision)) { this->activation_callback = activation_callback; }
    void SetDectivationCallback(void (*deactivation_callback)(TriggerComponent*, Physics::Collision)) { this->deactivation_callback = deactivation_callback; }
    void SetFilterCallback(bool (*filter_callback)(TriggerComponent*, PhysicsComponent*)) { this->filter_callback = filter_callback; }

    void SetCollisionMask(uint32_t flags);
    void SetCollisionGroup(uint32_t flags);
    
    uint32_t GetCollisionMask() { return collisionMask; }
    uint32_t GetCollisionGroup() { return collisionGroup; }
    
    void SetLocation(glm::vec3 location);
    void SetRotation(glm::quat rotation);
    
    void Collision (const Physics::Collision& collision);
    void ResetCollisions();
    
    std::vector<Physics::Collision> Poll ();
private:
    ResourceProxy<Physics::CollisionModel> model;
    
    uint32_t collisionMask = -1;
    uint32_t collisionGroup = -1;
    
    glm::vec3 location = vec3 (0.0f, 0.0f, 0.0f);
    glm::quat rotation = vec3 (0.0f, 0.0f, 0.0f);
    
    btCollisionShape* shape = nullptr;
    btCollisionObject* trigger = nullptr;
    
    void (*activation_callback)(TriggerComponent*, Physics::Collision) = nullptr;
    void (*deactivation_callback)(TriggerComponent*, Physics::Collision) = nullptr;
    bool (*filter_callback)(TriggerComponent*, PhysicsComponent*) = nullptr;

    bool is_collided = false;
    bool was_collided = false;
    
    Physics::Collision collision;
};

}

#endif // COMPONENTS_TRIGGERCOMPONENT_H