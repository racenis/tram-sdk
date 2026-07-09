// Tramway Drifting and Dungeon Exploration Simulator SDK Runtime

#ifndef TRAM_SDK_COMPONENTS_SPRITECOMPONENT_H
#define TRAM_SDK_COMPONENTS_SPRITECOMPONENT_H

#include <render/render.h>
#include <render/sprite.h>

#include <render/vertices.h>

namespace tram {

class SpriteComponent : public EntityComponent {
public:
    static SpriteComponent* Make();
    static void Yeet(SpriteComponent* component);
    
    inline Render::Sprite* GetSprite() const { return sprite.get(); }
    void SetSprite(Render::Sprite* sprite);

    void Update();
    
    inline void SetScale(float scale, int32_t index = 0) {
        Resize(index); infos[index].scale = {scale, scale};
        renderlist_needs_update = true;
    }
    
    inline void SetScale(vec2 scale, int32_t index = 0) {
        Resize(index); infos[index].scale = scale;
        renderlist_needs_update = true;
    }
    
    inline void SetFrame(int32_t frame, int32_t index = 0) {
        Resize(index); infos[index].anim_frame = frame;
        renderlist_needs_update = true;
    }
    
    inline void SetSpeed(int32_t speed, int32_t index = 0) {
        Resize(index); infos[index].anim_speed = speed;
        renderlist_needs_update = true;
    }
    
    inline void SetFrameBegin(int32_t frame, int32_t index = 0) {
        Resize(index); infos[index].first_frame = frame;
        renderlist_needs_update = true;
    }
    
    inline void SetFrameEnd(int32_t frame, int32_t index = 0) {
        Resize(index); infos[index].last_frame = frame;
        renderlist_needs_update = true;
    }
    
    inline void Play(int32_t index = 0) {
        Resize(index);
        if (!infos[index].is_playing) sprites_playing++;
        infos[index].is_playing = true;
    }
    
    inline void Pause(int32_t index = 0) {
        Resize(index);
        if (infos[index].is_playing) sprites_playing--;
        infos[index].is_playing = false;
    }
  
    void SetColor(vec3 color, int32_t index = 0) {
        Resize(index); points[index].color = color;
        renderlist_needs_update = true;
    }

    inline void SetLocation(vec3 location, int32_t index = 0) {
        Resize(index); infos[index].location = location;
        renderlist_needs_update = true;
    }
    
    inline void SetRotation(float rotation, int32_t index = 0) {
        Resize(index); points[index].rotation = rotation;
        renderlist_needs_update = true;
    }
    
    void SetLayers(uint32_t layers) {
        this->layers = layers;
        renderlist_needs_update = true;
    }
    
    int32_t GetSpriteCount() { return sprite_count; }
    
    inline void SetSpriteCount(int32_t count) { Resize(count - 1, true); }

    void EventHandler(Event &event){return;}
protected:
    ResourceProxy<Render::Sprite> sprite;
    uint32_t layers = 1;
    
    struct SpriteInfo {
        int32_t anim_frame;
        int32_t anim_speed;
        
        int32_t first_frame;
        int32_t last_frame;
        
        vec3 location;
        vec2 scale;
        
        bool is_playing;
        
        int32_t since_last_anim_frame;
    };
    
    Render::SpritePoint* points = nullptr;
    SpriteInfo* infos = nullptr;
    int32_t sprite_count = 0;
    int32_t sprites_playing = 0;
    
    void Resize(int32_t frame, bool downside = false);
    
    bool renderlist_needs_update = false;
    
    void Start();
    void UpdateRenderListObject();
    
    SpriteComponent() : sprite(this) {}
    ~SpriteComponent();
    
    Render::drawlistentry_t draw_list_entry = {};
    Render::vertexarray_t vertex_array = {};
    Render::spritearray_t sprite_array = {};
};

}

#endif // TRAM_SDK_COMPONENTS_SPRITECOMPONENT_H