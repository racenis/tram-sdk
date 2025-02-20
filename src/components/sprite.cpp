// Tramway Drifting and Dungeon Exploration Simulator SDK Runtime

#include <render/api.h>
#include <render/vertices.h>
#include <render/material.h>
#include <components/sprite.h>
#include <framework/logging.h>

#include <config.h>

/**
 * @class tram::SpriteComponent components/sprite.h <components/sprite.h>
 * 
 * Provides amusing graphical effects.
 * @see https://racenis.github.io/tram-sdk/documentation/components/sprite.html
 */

namespace tram {
using namespace tram::Render;

template <> Pool<SpriteComponent> PoolProxy<SpriteComponent>::pool ("SpriteComponent pool", COMPONENT_LIMIT_SPRITE);
template <> void Component<SpriteComponent>::init() { ptr = PoolProxy<SpriteComponent>::New(); }
template <> void Component<SpriteComponent>::yeet() { PoolProxy<SpriteComponent>::Delete(ptr); }

using namespace API;

SpriteComponent::~SpriteComponent() {
    is_ready = false;

    RemoveDrawListEntry(draw_list_entry);
};

void SpriteComponent::Start() {
    assert(!is_ready);

    //CreateVertexArray(GetVertexDefinition(VERTEX_SPRITE), vertex_array);
    sprite_array = CreateSpriteArray();
    
    auto texture_handle = sprite->GetMaterial()->GetTexture();
    
    draw_list_entry = InsertDrawListEntry();
    //SetDrawListVertexArray(draw_list_entry, vertex_array);
    SetDrawListSpriteArray(draw_list_entry, sprite_array);
    SetDrawListIndexRange(draw_list_entry, 0, 6);
    SetFlags(draw_list_entry, FLAG_RENDER);
    SetDrawListTextures(draw_list_entry, 1, &texture_handle);
    SetDrawListShader(draw_list_entry, VERTEX_SPRITE, MATERIAL_TEXTURE_ALPHA);

    is_ready = true;
    UpdateRenderListObject();
}

void SpriteComponent::Update() {
    if (!is_ready) return;
    
    if (anim_isplaying) {
        UpdateRenderListObject();
    
        if (!(anim_bframe < anim_speed)) {
            anim_bframe = 0;
            anim_frame++;
            
            if (!(anim_frame < sprite->GetFrames().size())) anim_frame = 0;
        }
        anim_bframe++;
    }
}

void SpriteComponent::UpdateRenderListObject() {
    if (!is_ready) return;

    // maybe cache these values, instead of re-calculating them for each frame?
    float tex_width = (float)sprite->GetFrames()[anim_frame].width / (float)sprite->GetMaterial()->GetWidth();//sprite->width;
    float tex_height = (float)sprite->GetFrames()[anim_frame].height / (float)sprite->GetMaterial()->GetHeight(); //sprite->height;
    float tex_w_off = (float)sprite->GetFrames()[anim_frame].offset_x / (float)sprite->GetMaterial()->GetWidth();//(float)(anim_frame % sprite->frames_w) * tex_width;
    float tex_h_off = (float)sprite->GetFrames()[anim_frame].offset_y / (float)sprite->GetMaterial()->GetHeight();//(float)(anim_frame / sprite->frames_w) * tex_height;
    //float half_width = tex_width; //* sprite->GetFrames()[anim_frame].scale / 2.0f;
    //float half_height = tex_height; //* sprite->GetFrames()[anim_frame].scale / 2.0f;

    tex_h_off = 1.0f - tex_h_off - tex_height;

    Render::SpritePoint point;
    point.position = {0.0f, 0.0f, 0.0f};
    point.center = {0.0f, 0.0f, 0.0f};
    point.color = COLOR_WHITE;
    point.rotation = 0.0f;
    point.dimensions = {tex_width * 10.0f, tex_height * 10.0f};
    point.texture_offset = {tex_w_off, tex_h_off};
    point.texture_size = {tex_width, tex_height};
    point.texture = 0;

    UpdateSpriteArray(sprite_array, 1, &point);
    
    Render::API::SetMatrix(draw_list_entry, PositionRotationToMatrix(location, quat(1.0f, 0.0f, 0.0f, 0.0f)));
}

        
void SpriteComponent::Play() {
    anim_isplaying = true;
}

void SpriteComponent::Pause() {
    anim_isplaying = true;
}

void SpriteComponent::SetPlaySpeed (size_t speed) {
    anim_speed = speed;
}

}