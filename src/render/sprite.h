// TRAMWAY DRIFT AND DUNGEON EXPLORATION SIMULATOR 2022
// All rights reserved.

#ifndef RENDER_SPRITE_H
#define RENDER_SPRITE_H

#include <render/render.h>

namespace tram::Render {
    
class Material;

struct SpriteFrame {
    uint16_t offset_x;
    uint16_t offset_y;
    uint16_t width;
    uint16_t height;
    uint16_t drop;
    uint16_t border;
    float scale;
    float length;       // idk what this is supposed to be
};

class Sprite : public Resource {
public:
    Sprite (UID name) : Resource(name) {}
    inline Material* GetMaterial () const { return material; }
    inline void SetMaterial (Material* mat) { material = mat; }
    
    void LoadFromDisk();
    void LoadFromMemory();
    
    void Unload() {}

    const std::vector<SpriteFrame>& GetFrames() { return frames; };

    static Sprite* Find(name_t name);
protected:
    std::vector<SpriteFrame> frames;
    Material* material = nullptr;
};
    
}

#endif // RENDER_SPRITE_H