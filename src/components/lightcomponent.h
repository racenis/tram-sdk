// TRAMWAY DRIFT AND DUNGEON EXPLORATION SIMULATOR 2022
// All rights reserved.
//
// LIGHTCOMPONENTS.H - Light source component
// A wrapper around the light concept in the rendering engine.

#ifndef LIGHTCOMPONENT_H
#define LIGHTCOMPONENT_H

#include <render/render.h>

namespace Core {
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
            } else {
                std::cout << "Can't update unloaded light." << std::endl;
            }
        };
        void UpdateLocation(float loc1, float loc2, float loc3){
            location[0] = loc1;
            location[1] = loc2;
            location[2] = loc3;
            Update();
        };
        void UpdateColor(float col1, float col2, float col3){
            color[0] = col1;
            color[1] = col2;
            color[2] = col3;
            Update();
        };
        void UpdateDistance(float dist){
            distance = dist;
            Update();
        }
        void GetColor(float (&cd)[3]){
            cd[0] = color[0];
            cd[1] = color[1];
            cd[2] = color[2];
        }
        void GetDistance(float& dist){
            dist = distance;
        }
        bool Remind(){return true;}// TODO: Yeet this
        void Init(){lightLObj = Render::lightPool.AddNew(); light_tree_id = Render::lightTree.AddLeaf(lightLObj - Render::lightPool.begin().ptr, location.x, location.y, location.z); is_ready = true; Update();};
        void Uninit(){Render::lightPool.Remove(lightLObj); Render::lightTree.RemoveLeaf(light_tree_id); lightLObj = nullptr;};
        void Start(){}
        void EventHandler(Event &event){return;}
    };
}

#endif //LIGHTCOMPONENT_H