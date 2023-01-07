// TRAMWAY DRIFT AND DUNGEON EXPLORATION SIMULATOR 2022
// All rights reserved.

#ifndef COMPONENTS_AICOMPONENT_H
#define COMPONENTS_AICOMPONENT_H

#include <framework/core.h>
#include <components/controllercomponent.h>

// this is just a sketch right now
// a few things still need to be finished until the component can be implemented:
// - PODPool (like stackpool but for PODs)
// - job system
// - navmesh route maker

namespace Core {
    class AIComponent : public EntityComponent {
    public:
        class Behaviour {
        public:
            // returns a weight with importance of the behavioure at the time of query
            // larger number means that the behaviour is more important
            virtual int32_t FindWeight() = 0;
            
            // updates the behaviour
            virtual bool Update() = 0;
            
            // resets the behaviour after it hasn't been updated in a while
            virtual void Reset() = 0;
        protected:
            AIComponent* parent;
        };
        
        void Init() override {
            assert(controller);
            
            // check what events the behaviours are interested in
            // then register event listeners for them
        }
        
        void Start() override {
            // do nothing, since we don't need any resources
        }
        
        void Update() {
            Behaviour* best_behaviour = nullptr;
            int32_t best_behaviour_weight = 0;
            
            for (auto& behaviour : behaviours) {
                int32_t weight = behaviour.FindWeight();
                if (weight > best_behaviour_weight) {
                    best_behaviour = &behaviour;
                    best_behaviour_weight = weight;
                }
            }
            
            if (best_behaviour && best_behaviour != behaviour_stack.top()) {
                *behaviour_stack.AddNew() = best_behaviour;
                behaviour_stack.top()->Reset();
            } else {
                behaviour_stack.top()->Update();
            }
        }
        
        inline void SetController(ControllerComponent* controller) { this->controller = controller; }
        
        inline ControllerComponent* GetController() { return this->controller; }

        void EventHandler(Event &event) override {}
    protected:
        Stack<Behaviour*> behaviour_stack;
        ControllerComponent* controller = nullptr;
        std::vector<Behaviour> behaviours;
        
        char event_data[100];
    };
    
    
    class AIWaitAroundDoingNothing : public AIComponent::Behaviour {
        AIWaitAroundDoingNothing(AIComponent* parent) { this->parent = parent; }
        
        ~AIWaitAroundDoingNothing() { /* do nothing */ }
        
        int32_t FindWeight() override {
            // no matter what, the importance of waiting around
            // and doing nothing will be very low
            return 10;
        }

        void Reset() override {
            // make the controller do a waiting animation
            parent->GetController()->Act(ControllerComponent::ACTION_LOOK_BUSY);
        }
        
        bool Update() override {
            // at this point the controller should already be set up to to
            // play a waiting animation, so we don't have to do anything
            
            // since the waiting will never terminate, we will always
            // return true
            return true;
        }
    };
    
}

#endif // COMPONENTS_AICOMPONENT_H