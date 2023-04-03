// TRAMWAY DRIFT AND DUNGEON EXPLORATION SIMULATOR 2022
// All rights reserved.

#ifndef COMPONENTS_LIGHTCOMPONENT_H
#define COMPONENTS_LIGHTCOMPONENT_H

#include <render/api.h>

namespace tram {
    
    class LightComponent : public EntityComponent {
    protected:
        Render::light_t light = nullptr;
        vec3 location;
        vec3 color;
        float distance;
    public:
        void Update(){
            if(is_ready){
                Render::SetLightParameters(light, location, color, distance);
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
        void Init(){ light = Render::MakeLight(); is_ready = true; Update(); }
        ~LightComponent(){Render::DeleteLight(light); light = nullptr;};
        void Start(){}
        void EventHandler(Event &event){return;}
    };
}

#endif // COMPONENTS_LIGHTCOMPONENT_H