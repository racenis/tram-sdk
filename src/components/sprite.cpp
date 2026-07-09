// Tramway Drifting and Dungeon Exploration Simulator SDK Runtime

#include <render/api.h>
#include <render/vertices.h>
#include <render/material.h>
#include <components/sprite.h>
#include <framework/logging.h>

#include <config.h>

#include <cstdlib>
#include <cstring>

/**
 * @class tram::SpriteComponent components/sprite.h <components/sprite.h>
 * 
 * Provides amusing graphical effects.
 * @see https://racenis.github.io/tram-sdk/documentation/components/sprite.html
 */

namespace tram {
using namespace tram::Render;

template <> Pool<SpriteComponent> PoolProxy<SpriteComponent>::pool("SpriteComponent pool", COMPONENT_LIMIT_SPRITE);
template <> void Component<SpriteComponent>::init() { ptr = SpriteComponent::Make(); }
template <> void Component<SpriteComponent>::yeet() { SpriteComponent::Yeet(ptr); }

using namespace API;

SpriteComponent::~SpriteComponent() {
    is_ready = false;
    
    if (infos) free(infos);
    if (points) free(points);
    
    RemoveDrawListEntry(draw_list_entry);
};

void SpriteComponent::SetSprite(Render::Sprite* sprite) {
    if (is_ready) {
        Log(Severity::WARNING, System::RENDER, "Initialized SpriteComponents cannot accept sprites! Ignoring SpriteComponent::SetSprite() call.");
        return;
    }
    
    this->sprite = sprite;    
}

void SpriteComponent::Resize(int32_t frame, bool downside) {
    int32_t want_count = frame + 1;
    if (sprite_count == want_count) return;
    
    if (!sprite_count && want_count > 0) {
        points = (Render::SpritePoint*)calloc(want_count, sizeof(Render::SpritePoint));
        infos = (SpriteInfo*)calloc(want_count, sizeof(SpriteInfo));
        sprite_count = want_count;
        for (int32_t i = 0; i < want_count; i++) {
            infos[i].scale = {1.0f, 1.0f};
        }
        return;
    }
    
    if (want_count < 1) {
        if (points) free(points);
        if (infos) free(infos);
        sprite_count = 0;
        return;
    }
    
    points = (Render::SpritePoint*)realloc(points, want_count * sizeof(Render::SpritePoint));
    infos = (SpriteInfo*)realloc(infos, want_count * sizeof(SpriteInfo));
    
    if (want_count > sprite_count) {
        memset(points + sprite_count, 0, (want_count - sprite_count) * sizeof(Render::SpritePoint));
        memset(infos + sprite_count, 0, (want_count - sprite_count) * sizeof(SpriteInfo));
        for (int32_t i = sprite_count; i < want_count; i++) {
            infos[i].scale = {1.0f, 1.0f};
        }
    }
    
    sprite_count = want_count;
}

void SpriteComponent::Start() {
    if (!sprite) {
        Log(Severity::CRITICAL_ERROR, System::RENDER, "Sprite component doesn't have its model set!");
    }

    sprite_array = CreateSpriteArray();
    
    auto texture_handle = sprite->GetMaterial()->GetMaterial();
    
    draw_list_entry = InsertDrawListEntry();
    SetDrawListSpriteArray(draw_list_entry, sprite_array);
    SetDrawListIndexRange(draw_list_entry, 0, 6);
    SetLayer(draw_list_entry, layers);
    SetFlags(draw_list_entry, FLAG_RENDER);
    SetDrawListMaterials(draw_list_entry, 1, &texture_handle);
    SetDrawListShader(draw_list_entry, VERTEX_SPRITE, MATERIAL_TEXTURE_ALPHA);

    is_ready = true;
    UpdateRenderListObject();
}

void SpriteComponent::Update() {
    if (!is_ready) return;
    
    if (sprites_playing) {
        for (int32_t i = 0; i < sprite_count; i++) {
            if (++infos[i].since_last_anim_frame > infos[i].anim_speed) {
                infos[i].since_last_anim_frame = 0;
                infos[i].anim_frame++;
                if (infos[i].anim_frame >= infos[i].last_frame) {
                    infos[i].anim_frame = infos[i].first_frame;
                }
                renderlist_needs_update = true;
            }
        }
    }
    
    if (renderlist_needs_update) {
        UpdateRenderListObject();
    }
}

void SpriteComponent::UpdateRenderListObject() {
    if (!is_ready) return;
    
    vec3 midpoint = {0.0f, 0.0f, 0.0f};
    for (int32_t i = 0; i < sprite_count; i++) {
        midpoint += infos[i].location;
    }
    if (sprite_count) midpoint /= sprite_count;
    
    for (int32_t i = 0; i < sprite_count; i++) {
        float tex_width = (float)sprite->GetFrames()[infos[i].anim_frame].width / (float)sprite->GetMaterial()->GetWidth();
        float tex_height = (float)sprite->GetFrames()[infos[i].anim_frame].height / (float)sprite->GetMaterial()->GetHeight();
        float tex_w_off = (float)sprite->GetFrames()[infos[i].anim_frame].offset_x / (float)sprite->GetMaterial()->GetWidth();
        float tex_h_off = (float)sprite->GetFrames()[infos[i].anim_frame].offset_y / (float)sprite->GetMaterial()->GetHeight();
        
        float wrld_x_mid = (float)sprite->GetFrames()[infos[i].anim_frame].midpoint_x / 32.0f;
        float wrld_y_mid = (float)sprite->GetFrames()[infos[i].anim_frame].midpoint_y / 32.0f;
        
        float wrld_width = (float)sprite->GetFrames()[infos[i].anim_frame].width / 32.0f;
        float wrld_height = (float)sprite->GetFrames()[infos[i].anim_frame].height / 32.0f;

        tex_h_off = 1.0f - tex_h_off - tex_height;
        
        points[i].position = infos[i].location - midpoint;
        points[i].dimensions = {wrld_width * infos[i].scale.x, wrld_height * infos[i].scale.y};
        points[i].midpoint = {wrld_x_mid * infos[i].scale.x, wrld_y_mid * infos[i].scale.y};
        points[i].texture_offset = {tex_w_off, tex_h_off};
        points[i].texture_size = {tex_width, tex_height};
    }

    UpdateSpriteArray(sprite_array, sprite_count, points);
    
    SetDrawListIndexRange(draw_list_entry, 0, sprite_count * 6);
    Render::API::SetMatrix(draw_list_entry, PositionRotationToMatrix(midpoint, quat(1.0f, 0.0f, 0.0f, 0.0f)));
}

/// Creates a new SpriteComponent.
SpriteComponent* SpriteComponent::Make() {
    SpriteComponent* ptr = PoolProxy<SpriteComponent>::GetPool().allocate();
    new(ptr) SpriteComponent();
    return ptr;
}

/// Deletes an SpriteComponent.
void SpriteComponent::Yeet(SpriteComponent* component) {
    component->~SpriteComponent();
    PoolProxy<SpriteComponent>::GetPool().deallocate(component);
}

}