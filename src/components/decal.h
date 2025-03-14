// Tramway Drifting and Dungeon Exploration Simulator SDK Runtime

#ifndef TRAM_SDK_COMPONENTS_DECALCOMPONENT_H
#define TRAM_SDK_COMPONENTS_DECALCOMPONENT_H

#include <render/render.h>
#include <render/sprite.h>

namespace tram {

class DecalComponent : public EntityComponent {
public:
    DecalComponent() : sprite(this) {}
    ~DecalComponent();
    inline name_t GetSprite() { return sprite->GetName(); }

    void SetSprite(Render::Sprite* sprite) {
        this->sprite = sprite;
    }

    void Start();
    
    void Update();

    void UpdateRenderListObject();

    void SetScale(float scale) {
        this->scale = scale;
    }

    void SetFrame(uint32_t frame) {
        this->frame = frame;
    }

    void SetLocation(vec3 location) {
        this->location = location;
        UpdateRenderListObject();
    }
    
    void SetRotation(quat rotation) {
        this->rotation = rotation;
        UpdateRenderListObject();
    }

    void EventHandler(Event &event){return;}
protected:
    ResourceProxy<Render::Sprite> sprite;

    vec3 location = {0.0f, 0.0f, 0.0f};
    quat rotation = {1.0f, 0.0f, 0.0f, 0.0f};
    
    float scale = 1.0f;
    
    uint32_t frame = 0;

    Render::drawlistentry_t draw_list_entry = {};
    Render::vertexarray_t vertex_array = {};
};

}

#endif // TRAM_SDK_COMPONENTS_DECALCOMPONENT_H