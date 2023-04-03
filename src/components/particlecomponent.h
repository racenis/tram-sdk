// TRAMWAY DRIFT AND DUNGEON EXPLORATION SIMULATOR 2022
// All rights reserved.

#ifndef COMPONENTS_PARTICLECOMPONENT_H
#define COMPONENTS_PARTICLECOMPONENT_H

#include <render/render.h>
#include <render/sprite.h>

namespace tram {

class ParticleComponent : public EntityComponent {
public:
    struct Particle {
        glm::vec3 coords;
        glm::vec3 velocity;
        uint32_t age;
    };
    
    ParticleComponent() : sprite(this){}
    ~ParticleComponent();
    inline name_t GetSprite(){return sprite->GetName();}

    void SetSprite(Render::Sprite* sprite){
        this->sprite = sprite;
    }

    void Start();
    
    void Update();

    void UpdateRenderListObject();

    void UpdateLocation(glm::vec3 nlocation){
        location = nlocation;
        UpdateRenderListObject();
    }

    void EventHandler(Event &event){return;}
    
    void EmitParticle (const Particle& particle);

    // put these behind getter/setter methods, maybe?
    uint32_t emission_rate = 1;
    uint32_t particle_max_age = 120;
    glm::vec3 gravity = glm::vec3(0.0f, -0.005f, 0.0f);
    glm::vec3 initial_velocity = glm::vec3(0.0f, 0.15f, 0.0f);
    float initial_velocity_randomness = 0.1f;

protected:
    
    std::vector<Particle> particles;
    
    ResourceProxy<Render::Sprite> sprite;

    //Render::RenderListObject* robject = nullptr;
    Render::drawlistentry_t draw_list_entry;
    
    glm::vec3 location;
    
    uint32_t vertex_array;
    uint32_t vertex_buffer;
};

}

#endif // COMPONENTS_PARTICLECOMPONENT_H