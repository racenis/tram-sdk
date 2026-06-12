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
    inline Render::Sprite* GetSprite() { return sprite.get(); }

    void SetSprite(Render::Sprite* sprite){
        this->sprite = sprite;
    }
    
    void Update();
    
    void Play();
    void Pause();
    
    void SetPlaySpeed(size_t speed);

    void UpdateLocation(vec3 nlocation){
        location = nlocation;
        UpdateRenderListObject();
    }
    
    void SetRotation(float rotation) {
        this->rotation = rotation;
        UpdateRenderListObject();
    }

    void EventHandler(Event &event){return;}
protected:
    ResourceProxy<Render::Sprite> sprite;

    vec3 location;
    float rotation;
    
    size_t anim_frame = 0;
    size_t anim_speed = 0;
    size_t anim_bframe = 0;
    bool anim_isplaying = false;
    
    void Start();
    void UpdateRenderListObject();
    
    Render::drawlistentry_t draw_list_entry = {};
    Render::vertexarray_t vertex_array = {};
    Render::spritearray_t sprite_array = {};
};

}

#endif // TRAM_SDK_COMPONENTS_SPRITECOMPONENT_H