// Tramway Drifting and Dungeon Exploration Simulator SDK Runtime

#include <render/api.h>
#include <render/vertices.h>
#include <components/particle.h>
#include <render/material.h>

#include <config.h>

/**
 * @class tram::ParticleComponent components/particle.h <components/particle.h>
 * 
 * Provides amusing graphical effects.
 * @see https://racenis.github.io/tram-sdk/documentation/components/particle.html
 */

namespace tram {

using namespace tram::Render;
using namespace tram::Render::API;

template <> Pool<ParticleComponent> PoolProxy<ParticleComponent>::pool("ParticleComponent pool", COMPONENT_LIMIT_PARTICLE, false);
template <> void Component<ParticleComponent>::init() { ptr = PoolProxy<ParticleComponent>::New(); }
template <> void Component<ParticleComponent>::yeet() { PoolProxy<ParticleComponent>::Delete(ptr); }

ParticleComponent::~ParticleComponent(){
    is_ready = true;

    RemoveDrawListEntry(draw_list_entry);
};

void ParticleComponent::EmitParticle (const Particle& particle) {
    particles.push_back(particle);
}

void ParticleComponent::Start() {
    if(is_ready) return;

    CreateVertexArray(GetVertexDefinition(VERTEX_SPRITE), vertex_array);

    auto texture_handle = sprite->GetMaterial()->GetTexture();
    
    draw_list_entry = InsertDrawListEntry();
    SetDrawListVertexArray(draw_list_entry, vertex_array);
    SetDrawListIndexRange(draw_list_entry, 0, 6);
    SetFlags(draw_list_entry, FLAG_RENDER);
    SetDrawListTextures(draw_list_entry, 1, &texture_handle);
    SetDrawListShader(draw_list_entry, VERTEX_SPRITE, MATERIAL_TEXTURE_ALPHA);
    
    is_ready = true;
    UpdateRenderListObject();
}

void ParticleComponent::Update(){
    if (!is_ready) return;
    
    for (auto it = particles.begin(); it < particles.end();) {
        it->velocity += gravity;
        it->coords += it->velocity;
        it->age++;
        
        if (it->age > particle_max_age) {
            it = particles.erase(it);
        } else {
            it++;
        }
    }
    
    for (uint32_t i = 0; i < emission_rate; i++) {
        vec3 random_vec;
        static int tick = 0;
        tick++; srand(tick*6217);
        random_vec.x = 0.5f - ((float)rand()/(float)RAND_MAX);
        tick++; srand(tick*6217);
        random_vec.y = 0.5f - ((float)rand()/(float)RAND_MAX);
        tick++; srand(tick*6217);
        random_vec.z = 0.5f - ((float)rand()/(float)RAND_MAX);
        
        vec3 velocity = initial_velocity + (random_vec*initial_velocity_randomness);
        particles.push_back(Particle {velocity, velocity, 0});
    }

    
    UpdateRenderListObject();
}

void ParticleComponent::UpdateRenderListObject(){
    if (!is_ready) return;
    
    float tex_width = sprite->GetFrames().front().width / (float)sprite->GetMaterial()->GetWidth();
    float tex_height = sprite->GetFrames().front().height / (float)sprite->GetMaterial()->GetHeight();
    float tex_w_off = sprite->GetFrames().front().offset_x / (float)sprite->GetMaterial()->GetWidth();
    float tex_h_off = sprite->GetFrames().front().offset_y / (float)sprite->GetMaterial()->GetHeight();
    float half_width = tex_width; //* sprite->GetFrames().front().scale / 2.0f;
    float half_height = tex_height; //* sprite->GetFrames().front().scale / 2.0f;

    std::vector<Render::SpriteVertex> vertices;

    for (auto particle : particles) {
        Render::SpriteVertex top_left {
            //.co = vec3(0.0f, 0.0f, 0.0f),
            .co = particle.coords,
            .voffset = vec2(-half_width, half_height),
            .texco = vec2(0.0f + tex_w_off, 1.0f - tex_h_off),
            .verticality = 1.0f,
            .texture = 0
        };
        
        Render::SpriteVertex top_right {
            //.co = vec3(0.0f, 0.0f, 0.0f),
            .co = particle.coords,
            .voffset = vec2(half_width, half_height),
            .texco = vec2(tex_width + tex_w_off, 1.0f - tex_h_off),
            .verticality = 1.0f,
            .texture = 0
        };
        
        Render::SpriteVertex bottom_left {
            //.co = vec3(0.0f, 0.0f, 0.0f),
            .co = particle.coords,
            .voffset = vec2(-half_width, -half_height),
            .texco = vec2(0.0f + tex_w_off, 1.0f - tex_height - tex_h_off),
            .verticality = 1.0f,
            .texture = 0
        };
        
        Render::SpriteVertex bottom_right {
            //.co = vec3(0.0f, 0.0f, 0.0f),
            .co = particle.coords,
            .voffset = vec2(half_width, -half_height),
            .texco = vec2(tex_width + tex_w_off, 1.0f - tex_height - tex_h_off),
            .verticality = 1.0f,
            .texture = 0
        };

        vertices.push_back(top_left);
        vertices.push_back(bottom_left);
        vertices.push_back(top_right);
        vertices.push_back(top_right);
        vertices.push_back(bottom_left);
        vertices.push_back(bottom_right);
    }
    
    UpdateVertexArray(vertex_array, sizeof(SpriteVertex) * vertices.size(), &vertices[0]);
    SetDrawListIndexRange(draw_list_entry, 0, /*6 * */vertices.size());
    
    Render::API::SetMatrix(draw_list_entry, PositionRotationToMatrix(location, quat(1.0f, 0.0f, 0.0f, 0.0f)));
}
    
}