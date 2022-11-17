// TRAMWAY DRIFT AND DUNGEON EXPLORATION SIMULATOR 2022
// All rights reserved.
//
// PARTICLECOMPONENT.CPP -- implementation of ParticleComponent

#include <render/renderer.h>
#include <components/particlecomponent.h>

namespace Core {
    using namespace Core::Render;
    
    ParticleComponent::~ParticleComponent(){
        is_ready = true;

        OpenGL::RemoveDrawListEntry(draw_list_entry);
    };

    void ParticleComponent::Start(){
        if(is_ready) return;

        OpenGL::CreateVertexArray(SPRITE_VERTEX_DEFINITION, vertex_buffer, vertex_array);

        auto texture_handle = sprite->GetMaterial()->GetTexture();
        
        draw_list_entry = OpenGL::InsertDrawListEntry();
        OpenGL::SetDrawListVertexArray(draw_list_entry, vertex_array);
        OpenGL::SetDrawListElements(draw_list_entry, 0, 6);
        OpenGL::SetFlags(draw_list_entry, FLAG_RENDER);
        OpenGL::SetDrawListTextures(draw_list_entry, 1, &texture_handle);
        OpenGL::SetDrawListShader(draw_list_entry, Model::SPRITE_VERTEX, Material::TEXTURE_ALPHA);
        
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
        
        glm::vec3 random_vec;
        static int tick = 0;
        tick++; srand(tick*6217);
        random_vec.x = 0.5f - ((float)rand()/(float)RAND_MAX);
        tick++; srand(tick*6217);
        random_vec.y = 0.5f - ((float)rand()/(float)RAND_MAX);
        tick++; srand(tick*6217);
        random_vec.z = 0.5f - ((float)rand()/(float)RAND_MAX);
        
        glm::vec3 velocity = initial_velocity + (random_vec*initial_velocity_randomness);
        particles.push_back(Particle {velocity, velocity, 0});
        
        UpdateRenderListObject();
    }

    void ParticleComponent::UpdateRenderListObject(){
        if (!is_ready) return;
        
        float tex_width = sprite->frames.front().width / (float)sprite->GetMaterial()->GetWidth();
        float tex_height = sprite->frames.front().height / (float)sprite->GetMaterial()->GetHeight();
        float tex_w_off = sprite->frames.front().offset_x / (float)sprite->GetMaterial()->GetWidth();
        float tex_h_off = sprite->frames.front().offset_y / (float)sprite->GetMaterial()->GetHeight();
        float half_width = tex_width * sprite->frames.front().scale / 2.0f;
        float half_height = tex_height * sprite->frames.front().scale / 2.0f;

        std::vector<Render::SpriteVertex> vertices;

        for (auto particle : particles) {
            Render::SpriteVertex top_left {
                //.co = glm::vec3(0.0f, 0.0f, 0.0f),
                .co = particle.coords,
                .voffset = glm::vec2 (-half_width, half_height),
                .texco = glm::vec2 (0.0f + tex_w_off, 1.0f - tex_h_off),
                .verticality = 1.0f,
                .texture = 0
            };
            
            Render::SpriteVertex top_right {
                //.co = glm::vec3(0.0f, 0.0f, 0.0f),
                .co = particle.coords,
                .voffset = glm::vec2 (half_width, half_height),
                .texco = glm::vec2 (tex_width + tex_w_off, 1.0f - tex_h_off),
                .verticality = 1.0f,
                .texture = 0
            };
            
            Render::SpriteVertex bottom_left {
                //.co = glm::vec3(0.0f, 0.0f, 0.0f),
                .co = particle.coords,
                .voffset = glm::vec2 (-half_width, -half_height),
                .texco = glm::vec2 (0.0f + tex_w_off, 1.0f - tex_height - tex_h_off),
                .verticality = 1.0f,
                .texture = 0
            };
            
            Render::SpriteVertex bottom_right {
                //.co = glm::vec3(0.0f, 0.0f, 0.0f),
                .co = particle.coords,
                .voffset = glm::vec2 (half_width, -half_height),
                .texco = glm::vec2 (tex_width + tex_w_off, 1.0f - tex_height - tex_h_off),
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
        
        OpenGL::UpdateVertexArray(vertex_buffer, sizeof(SpriteVertex) * vertices.size(), &vertices[0]);
        OpenGL::SetDrawListElements(draw_list_entry, 0, /*6 * */vertices.size());
        
        
        OpenGL::SetLocation(draw_list_entry, location);
    }
    
}