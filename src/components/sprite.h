// Tramway Drifting and Dungeon Exploration Simulator SDK Runtime


#ifndef TRAM_SDK_COMPONENTS_SPRITECOMPONENT_H
#define TRAM_SDK_COMPONENTS_SPRITECOMPONENT_H

#include <render/render.h>
#include <render/sprite.h>

namespace tram {

class SpriteComponent : public EntityComponent {
public:
    SpriteComponent() : sprite(this){}
    ~SpriteComponent();
    inline name_t GetSprite(){ return sprite->GetName(); }

    void SetSprite(Render::Sprite* sprite){
        this->sprite = sprite;
    }

    void Start();
    
    void Update();
    
    void Play();
    
    void Pause();
    
    void SetPlaySpeed(size_t speed);

    void UpdateRenderListObject();

    void UpdateLocation(vec3 nlocation){
        location = nlocation;
        UpdateRenderListObject();
    }

    void EventHandler(Event &event){return;}

protected:
    ResourceProxy<Render::Sprite> sprite;

    vec3 location;
    
    size_t anim_frame = 0;
    size_t anim_speed = 0;
    size_t anim_bframe = 0;
    bool anim_isplaying = false;
    
    Render::drawlistentry_t draw_list_entry = {.generic = nullptr};
    Render::vertexarray_t vertex_array = {.generic = nullptr};
    Render::spritearray_t sprite_array = {.generic = nullptr};
};

}

#endif // TRAM_SDK_COMPONENTS_SPRITECOMPONENT_H