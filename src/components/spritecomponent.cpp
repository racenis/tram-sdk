// TRAMWAY DRIFT AND DUNGEON EXPLORATION SIMULATOR 2022
// All rights reserved.

#include <render/renderer.h>
#include <components/spritecomponent.h>

namespace Core {
    using namespace Core::Render;
    
    SpriteComponent::~SpriteComponent(){
        is_ready = false;

        RemoveDrawListEntry(draw_list_entry);
    };

    void SpriteComponent::Start(){
        assert(!is_ready);

        CreateVertexArray(SPRITE_VERTEX_DEFINITION, vertex_buffer, vertex_array);
        
        auto texture_handle = sprite->GetMaterial()->GetTexture();
        
        draw_list_entry = InsertDrawListEntry();
        SetDrawListVertexArray(draw_list_entry, vertex_array);
        SetDrawListElements(draw_list_entry, 0, 6);
        SetFlags(draw_list_entry, FLAG_RENDER);
        SetDrawListTextures(draw_list_entry, 1, &texture_handle);
        SetDrawListShader(draw_list_entry, Model::SPRITE_VERTEX, Material::TEXTURE_ALPHA);

        is_ready = true;
        UpdateRenderListObject();
    }
    
    void SpriteComponent::Update(){
        if (!is_ready) return;
        
        if (anim_isplaying) {
            UpdateRenderListObject();
        
            if (!(anim_bframe < anim_speed)) {
                anim_bframe = 0;
                anim_frame++;
                if (!(anim_frame < sprite->frames.size())) anim_frame = 0;
            }
            anim_bframe++;
        }
    }

    void SpriteComponent::UpdateRenderListObject(){
        if (!is_ready) return;

        // maybe cache these values, instead of re-calculating them for each frame?
        float tex_width = (float)sprite->frames[anim_frame].width / (float)sprite->GetMaterial()->GetWidth();//sprite->width;
        float tex_height = (float)sprite->frames[anim_frame].height / (float)sprite->GetMaterial()->GetHeight(); //sprite->height;
        float tex_w_off = (float)sprite->frames[anim_frame].offset_x / (float)sprite->GetMaterial()->GetWidth();//(float)(anim_frame % sprite->frames_w) * tex_width;
        float tex_h_off = (float)sprite->frames[anim_frame].offset_y / (float)sprite->GetMaterial()->GetHeight();//(float)(anim_frame / sprite->frames_w) * tex_height;
        float half_width = tex_width * sprite->frames[anim_frame].scale / 2.0f;
        float half_height = tex_height * sprite->frames[anim_frame].scale / 2.0f;

        Render::SpriteVertex top_left {
            .co = glm::vec3(0.0f, 0.0f, 0.0f),
            .voffset = glm::vec2 (-half_width, half_height),
            .texco = glm::vec2 (0.0f + tex_w_off, 1.0f - tex_h_off),
            .verticality = 1.0f,
            .texture = 0
        };
        
        Render::SpriteVertex top_right {
            .co = glm::vec3(0.0f, 0.0f, 0.0f),
            .voffset = glm::vec2 (half_width, half_height),
            .texco = glm::vec2 (tex_width + tex_w_off, 1.0f - tex_h_off),
            .verticality = 1.0f,
            .texture = 0
        };
        
        Render::SpriteVertex bottom_left {
            .co = glm::vec3(0.0f, 0.0f, 0.0f),
            .voffset = glm::vec2 (-half_width, -half_height),
            .texco = glm::vec2 (0.0f + tex_w_off, 1.0f - tex_height - tex_h_off),
            .verticality = 1.0f,
            .texture = 0
        };
        
        Render::SpriteVertex bottom_right {
            .co = glm::vec3(0.0f, 0.0f, 0.0f),
            .voffset = glm::vec2 (half_width, -half_height),
            .texco = glm::vec2 (tex_width + tex_w_off, 1.0f - tex_height - tex_h_off),
            .verticality = 1.0f,
            .texture = 0
        };

        std::vector<Render::SpriteVertex> vertices;
        
        vertices.push_back(top_left);
        vertices.push_back(bottom_left);
        vertices.push_back(top_right);
        vertices.push_back(top_right);
        vertices.push_back(bottom_left);
        vertices.push_back(bottom_right);

        UpdateVertexArray(vertex_buffer, sizeof(SpriteVertex) * vertices.size(), &vertices[0]);
        
        SetLocation(draw_list_entry, location);
    }
    
            
    void SpriteComponent::Play() {
        anim_isplaying = true;
    }
    
    void SpriteComponent::Pause() {
        anim_isplaying = true;
    }
    
    void SpriteComponent::SetPlaySpeed(size_t speed) {
        anim_speed = speed;
    }
    
}