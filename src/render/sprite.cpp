// TRAMWAY DRIFT AND DUNGEON EXPLORATION SIMULATOR 2022
// All rights reserved.

#include <render/sprite.h>
#include <render/renderer.h>
#include <render/material.h>

#include <framework/async.h>
#include <framework/logging.h>
#include <framework/file.h>

#include <templates/hashmap.h>

#include <fstream>
#include <sstream>

using namespace tram;
using namespace tram::Render;

Hashmap<Sprite*> sprite_list ("SPRITE_LIST", 500);

Sprite* Sprite::Find(name_t name){
    auto sprite = sprite_list.Find(name);
    
    if (!sprite) {
        sprite = new Sprite (name);
        sprite_list.Insert(name, sprite);
    }

    return sprite;
}

void Sprite::LoadFromMemory() {
    assert(status == LOADED);
    status = READY;
}

void Sprite::LoadFromDisk() {
    std::string filename = std::string("data/sprites/") + std::string(name) + ".spr";
    
    File file (filename.c_str(), MODE_READ);
    
    if (!file.is_open()) {
        Log("Sprite not found: {}", filename);
        
        frames.push_back({
            0, 0,
            64, 64,
            32, 32,
            0, 0
        });
        
        if (!material) material = Material::Find("defaulttexture");
        
        status = LOADED;
        load_fail = true;
        
        return;
    }
    
    name_t header = file.read_name();
    name_t material_name = file.read_name();
    
    if (header != "SPRv2") {
        Log("Incorrect sprite header \"{}\" in file \"{}\"", header, filename);
    }
    
    while (file.is_continue()) {
        frames.push_back ({
            .offset_x = file.read_uint16(),
            .offset_y = file.read_uint16(),
            .width = file.read_uint16(),
            .height = file.read_uint16(),
            .midpoint_x = file.read_uint16(),
            .midpoint_y = file.read_uint16(),
            .border_h = file.read_uint16(),
            .border_v = file.read_uint16()
        });
    }
    
    if (!material) {
        material = Material::Find(UID(material_name));
    }
    
    material->AddReference();
    Async::LoadDependency(material);
    
    status = LOADED;
}

