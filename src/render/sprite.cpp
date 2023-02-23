// TRAMWAY DRIFT AND DUNGEON EXPLORATION SIMULATOR 2022
// All rights reserved.

#include <render/sprite.h>
#include <render/renderer.h>

#include <framework/async.h>
#include <framework/logging.h>

#include <templates/hashmap.h>

#include <fstream>
#include <sstream>

using namespace Core;
using namespace Core::Render;

//std::unordered_map<uint64_t, Sprite*> SPRITE_LIST;
Hashmap<Sprite*> SPRITE_LIST ("SPRITE_LIST", 500);

Sprite* Sprite::Find(name_t name){
    /*std::unordered_map<uint64_t, Sprite*>::iterator ff = SPRITE_LIST.find(name.key);
    Sprite* sprite;

    if (ff == SPRITE_LIST.end()) {
        sprite = new Sprite(name.key);
        SPRITE_LIST[name.key] = sprite;
    } else {
        sprite = ff->second;
    }*/
    
    auto sprite = SPRITE_LIST.Find(name);
    
    if (!sprite) {
        sprite = new Sprite (name);
        SPRITE_LIST.Insert(name, sprite);
    }

    return sprite;
}

void Sprite::LoadFromMemory() {
    assert(status == LOADED);
    status = READY;
}

void Sprite::LoadFromDisk() {
    if (frames.size() || !name) {
        status = LOADED;
        Log("Already loaded!");
        return;
    }
    
    std::string filename = std::string("data/sprites/") + std::string(name) + ".spr";
    std::ifstream file (filename);
    std::string str; 
    
    if (!file.is_open()) {
        Log("File not found: {}", filename);
    }
    
    std::getline(file, str);
    std::stringstream wrd (str);
    std::string type;
    std::string matname;
    
    wrd >> type;
    wrd >> matname;
    
    if (type != "SPRv1") {
        Log("Incorrect sprite header \"{}\" in file \"{}\"", str, filename);
    }
    
    while (std::getline(file, str)) {
        size_t first_nospace = str.find_first_not_of(" \t");
        if(first_nospace == std::string::npos || str[first_nospace] == '#') continue;
        std::stringstream wrd (str);
        
        Frame fr;
        
        wrd >> fr.offset_x;
        wrd >> fr.offset_y;
        wrd >> fr.width;
        wrd >> fr.height;
        wrd >> fr.drop;
        wrd >> fr.border;
        wrd >> fr.scale;
        wrd >> fr.length; 
        
        frames.push_back(fr);
    }
    
    if (!material) {
        material = Material::Find(UID(matname));
    }
    
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