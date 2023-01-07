// TRAMWAY DRIFT AND DUNGEON EXPLORATION SIMULATOR 2022
// All rights reserved.
//
// TRIGGERCOMPONENT.H -- Trigger component [kinda works]

#ifndef COMPONENTS_TRIGGERCOMPONENT_H
#define COMPONENTS_TRIGGERCOMPONENT_H

#include <physics/physics.h>
#include <components/physicscomponent.h>

// TODO: add adding a location
// TODO: add properly loading the model

namespace Core {
    class TriggerComponent : public EntityComponent {
    public:
        TriggerComponent() : model(this){}
        ~TriggerComponent();
        void EventHandler(Event &event){};
        void Start();

        name_t GetModel(){ return model->GetName(); }
        Entity* GetParent(){ return parent; }
        void SetModel(name_t modelName){ model = Physics::CollisionModel::Find(modelName); }
        void SetParent(Entity* newparent){ parent = newparent; }

        void SetActivationCallback(void (*activation_callback)(TriggerComponent*)) { this->activation_callback = activation_callback; }
        void SetFilterCallback(bool (*filter_callback)(TriggerComponent*, PhysicsComponent*)) { this->filter_callback = filter_callback; }

        void SetCollisionMask(uint32_t flags);
        void SetCollisionGroup(uint32_t flags);
        
        // TODO: make these move the trigger after init
        void SetLocation(glm::vec3 location) { this->location = location; }
        void SetRotation(glm::quat rotation) { this->rotation = rotation; }

        bool IsTriggered();
        
        void Collision(PhysicsComponent* other);
        void ResetCollisions();

    private:
        Entity* parent = nullptr;
        ResourceProxy<Physics::CollisionModel> model;
        
        glm::vec3 location;
        glm::quat rotation;
        
        btCollisionShape* shape = nullptr;
        btCollisionObject* trigger = nullptr;
        
        void (*activation_callback)(TriggerComponent*) = nullptr;
        bool (*filter_callback)(TriggerComponent*, PhysicsComponent*) = nullptr;
        
        uint32_t collisionMask = -1;
        uint32_t collisionGroup = 1;

        bool is_collided = false;
        bool was_collided = false;
    };
}

#endif // COMPONENTS_TRIGGERCOMPONENT_H