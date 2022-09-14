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