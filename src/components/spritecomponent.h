// TRAMWAY DRIFT AND DUNGEON EXPLORATION SIMULATOR 2022
// All rights reserved.
//
// SPRITECOMPONENT.H -- Sprite rendering component
// This component will render a single sprite onto the screen.

#ifndef SPRITECOMPONENT_H
#define SPRITECOMPONENT_H

#include <render.h>

namespace Core {
    class SpriteComponent : public EntityComponent {
    public:
        SpriteComponent() : sprite(this){}
        ~SpriteComponent() = default;
        inline name_t GetSprite(){return sprite->GetName();}

        void SetSprite(name_t name){
            sprite.SetResource(Render::Sprite::Find(name));
        }

        void Init(){
            is_ready = false; if(resources_waiting == 0) Start();
        }

        void Uninit();

        void Start();

        void UpdateRenderListObject();

        void UpdateLocation(glm::vec3 nlocation){
            location = nlocation;
            UpdateRenderListObject();
        }

        void EventHandler(Event &event){return;}

    protected:
        ResourceProxy<Render::Sprite> sprite;
    
        Render::RenderListObject* robject = nullptr;
        
        glm::vec3 location;
    };
}

#endif //SPRITECOMPONENT_H