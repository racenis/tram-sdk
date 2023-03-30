// TRAMWAY DRIFT AND DUNGEON EXPLORATION SIMULATOR 2022
// All rights reserved.

#ifndef COMPONENTS_LIGHTCOMPONENT_H
#define COMPONENTS_LIGHTCOMPONENT_H

#include <render/renderer.h>

namespace tram {
    
    class LightComponent : public EntityComponent {
    protected:
        Render::LightListObject* lightLObj = nullptr;
        glm::vec3 location;
        glm::vec3 color;
        float distance;
        uint32_t light_tree_id = 0;
    public:
        void Update(){
            if(is_ready){
                lightLObj->location = location;
                lightLObj->color = color;
                lightLObj->distance = distance;
            }
        };
        void SetLocation(const glm::vec3& location){
            this->location = location;
            Update();
        };
        void UpdateColor(const glm::vec3& color){
            this->color = color;
            Update();
        };
        void UpdateDistance(float dist){
            distance = dist;
            Update();
        }
        void GetColor(glm::vec3& color){
            color = this->color;
        }
        void GetDistance(float& distance){
            distance = this->distance;
        }
        void Init(){lightLObj = Render::lightPool.AddNew(); light_tree_id = Render::lightTree.AddLeaf(lightLObj - Render::lightPool.begin().ptr, location.x, location.y, location.z); is_ready = true; Update();};
        ~LightComponent(){Render::lightPool.Remove(lightLObj); Render::lightTree.RemoveLeaf(light_tree_id); lightLObj = nullptr;};
        void Start(){}
        void EventHandler(Event &event){return;}
    };
}

#endif // COMPONENTS_LIGHTCOMPONENT_H