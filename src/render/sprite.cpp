// TRAMWAY DRIFT AND DUNGEON EXPLORATION SIMULATOR 2022
// All rights reserved.

#include <render/render.h>
#include <render/renderer.h>

#include <framework/async.h>

using namespace Core;
using namespace Core::Render;

std::unordered_map<uint64_t, Sprite*> SPRITE_LIST;

Sprite* Sprite::Find(name_t name){
    std::unordered_map<uint64_t, Sprite*>::iterator ff = SPRITE_LIST.find(name.key);
    Sprite* sprite;

    if (ff == SPRITE_LIST.end()) {
        sprite = new Sprite(name.key);
        SPRITE_LIST[name.key] = sprite;
    } else {
        sprite = ff->second;
    }

    return sprite;
}

void Sprite::LoadFromMemory() {
    assert(status == LOADED);
    status = READY;
}

void Sprite::LoadFromDisk() {
    assert(material);
    material->AddRef();
    Async::ForceLoadResource(material);
    status = LOADED;
}

void Sprite::AutogenTiledFrames(uint16_t frame_offset_x, uint16_t frame_offset_y, uint16_t frame_width, uint16_t frame_height, uint16_t frames_per_row, uint16_t frame_count, float scale, float length) {
    uint16_t current_offset_x = frame_offset_x;
    uint16_t current_offset_y = frame_offset_y;
    uint16_t current_row_count = 0;
    
    frames.clear();
    
    for (uint16_t i = 0; i < frame_count; i++) {
        frames.push_back(Frame {
            .offset_x = current_offset_x,
            .offset_y = current_offset_y,
            .width = frame_width,
            .height = frame_height,
            .scale = scale,
            .length = length
        });
        
        current_row_count++;
        if (current_row_count == frames_per_row) {
            current_row_count = 0;
            current_offset_x = frame_offset_x;
            current_offset_y += frame_height;
        } else {
            current_offset_x += frame_width;
        }
    }
}