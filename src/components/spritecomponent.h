// TRAMWAY DRIFT AND DUNGEON EXPLORATION SIMULATOR 2022
// All rights reserved.

#ifndef COMPONENTS_SPRITECOMPONENT_H
#define COMPONENTS_SPRITECOMPONENT_H

#include <render/render.h>

namespace Core {
    class SpriteComponent : public EntityComponent {
    public:
        SpriteComponent() : sprite(this){}
        ~SpriteComponent();
        inline name_t GetSprite(){return sprite->GetName();}

        void SetSprite(Render::Sprite* sprite){
            this->sprite = sprite;
        }

        void Start();
        
        void Update();
        
        void Play();
        
        void Pause();
        
        void SetPlaySpeed(size_t speed);

        void UpdateRenderListObject();

        void UpdateLocation(glm::vec3 nlocation){
            location = nlocation;
            UpdateRenderListObject();
        }

        void EventHandler(Event &event){return;}

    protected:
        ResourceProxy<Render::Sprite> sprite;
    
        Render::DrawListEntryHandle draw_list_entry;
        
        glm::vec3 location;
        
        size_t anim_frame = 0;
        size_t anim_speed = 0;
        size_t anim_bframe = 0;
        bool anim_isplaying = false;
        
        uint32_t vertex_array;
        uint32_t vertex_buffer;
    };
}

#endif // COMPONENTS_SPRITECOMPONENT_H