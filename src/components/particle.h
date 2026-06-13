// Tramway Drifting and Dungeon Exploration Simulator SDK Runtime

#ifndef TRAM_SDK_COMPONENTS_PARTICLECOMPONENT_H
#define TRAM_SDK_COMPONENTS_PARTICLECOMPONENT_H

#include <render/render.h>
#include <render/particle.h>

namespace tram {

class ParticleComponent : public EntityComponent {
public:
    static ParticleComponent* Make();
    static void Yeet(ParticleComponent* component);
    
    void Update();
    void UpdateRenderListObject();

    void UpdateLocation(vec3 location){
        this->location = location;
        UpdateRenderListObject();
    }

    void SetParticle(Render::Particle* particle) {this->particle = particle;} 

    void EventHandler(Event &event){return;}
protected:
    ResourceProxy<Render::Particle> particle;

    vec3 location = {0.0f, 0.0f, 0.0f};

    Render::drawlistentry_t draw_list_entry = {};
    Render::spritearray_t sprite_array = {};
    
    struct System {
        std::vector<bool> slots;
        float since_last_emit = 0.0f;
        Render::Particle::System* system = nullptr;
    };
    
    void PerformOperation(const Render::Particle::Operation* op, int offset, int count);
    void PerformConstraint(const Render::Particle::Constraint* ct, int system);
    void PerformEmit(const Render::Particle::Emitter* em, int system);
    
    float& AsScalar(Render::Particle::LookupInfo info, int index, Render::Particle::MergeDest dest = Render::Particle::MergeDest::ANY);
    vec3& AsVector(Render::Particle::LookupInfo info, int index, Render::Particle::MergeDest dest = Render::Particle::MergeDest::ANY);
    
    float AsScalar(const Render::Particle::Parameter& param, int index);
    vec3 AsVector(const Render::Particle::Parameter& param, int index);
    
    void MergeIn(Render::Particle::LookupInfo info, int index, Render::Particle::MergeType type, Render::Particle::MergeDest dest, vec3 value);
    void MergeIn(Render::Particle::LookupInfo info, int index, Render::Particle::MergeType type, Render::Particle::MergeDest dest, float value);
    
    std::vector<System> systems;

    float* data = nullptr;
    
    void Start();
    
    ParticleComponent();
    ~ParticleComponent();
};

}

#endif // TRAM_SDK_COMPONENTS_PARTICLECOMPONENT_H