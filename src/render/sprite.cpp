// TRAMWAY DRIFT AND DUNGEON EXPLORATION SIMULATOR 2022
// All rights reserved.
//
// SPRITE.CPP
// Methods for the sprite resource.

#include <render/render.h>

#include <async.h>

using namespace Core;
using namespace Core::Render;

std::unordered_map<uint64_t, Sprite*> Sprite::List;

// TODO: Implement batching with buckets.
// Each bucket has a list of up to 16 sprites that use that bucket.
// Every time a sprite is loaded, it finds itself a bucket that has a free slot
// and adds itself to the bucket. Conversely, when the sprite is unloaded, it
// removes itself from the slot.
// Each sprite also holds an index into the list of buckets and an index into
// the sprite list inside of its bucket.

Sprite* Sprite::Find(name_t name){
    std::unordered_map<uint64_t, Sprite*>::iterator ff = List.find(name);
    Sprite* sprite;

    if(ff == List.end()){
        sprite = new Sprite(name);
        List[name] = sprite;
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