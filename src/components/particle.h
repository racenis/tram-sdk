// TRAMWAY DRIFT AND DUNGEON EXPLORATION SIMULATOR 2022
// All rights reserved.

#ifndef TRAM_SDK_COMPONENTS_PARTICLECOMPONENT_H
#define TRAM_SDK_COMPONENTS_PARTICLECOMPONENT_H

#include <render/render.h>
#include <render/sprite.h>

namespace tram {

class ParticleComponent : public EntityComponent {
public:
    struct Particle {
        vec3 coords;
        vec3 velocity;
        uint32_t age;
    };
    
    ParticleComponent() : sprite(this) {}
    ~ParticleComponent();
    inline name_t GetSprite() { return sprite->GetName(); }

    void SetSprite(Render::Sprite* sprite) {
        this->sprite = sprite;
    }

    void Start();
    
    void Update();

    void UpdateRenderListObject();

    void UpdateLocation(vec3 nlocation){
        location = nlocation;
        UpdateRenderListObject();
    }

    void EventHandler(Event &event){return;}
    
    void EmitParticle (const Particle& particle);

    // put these behind getter/setter methods, maybe?
    uint32_t emission_rate = 1;
    uint32_t particle_max_age = 120;
    vec3 gravity = vec3(0.0f, -0.005f, 0.0f);
    vec3 initial_velocity = vec3(0.0f, 0.15f, 0.0f);
    float initial_velocity_randomness = 0.1f;
protected:
    std::vector<Particle> particles;
    ResourceProxy<Render::Sprite> sprite;

    vec3 location;

    Render::drawlistentry_t draw_list_entry = {.generic = nullptr};
    Render::vertexarray_t vertex_array = {.generic = nullptr};
};

}

#endif // TRAM_SDK_COMPONENTS_PARTICLECOMPONENT_H