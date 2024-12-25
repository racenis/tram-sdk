// Tramway Drifting and Dungeon Exploration Simulator SDK Runtime


#ifndef TRAM_SDK_RENDER_SPRITE_H
#define TRAM_SDK_RENDER_SPRITE_H

#include <render/render.h>

namespace tram::Render {
    
class Material;

struct SpriteFrame {
    uint16_t offset_x;
    uint16_t offset_y;
    uint16_t width;
    uint16_t height;
    uint16_t midpoint_x;
    uint16_t midpoint_y;
    uint16_t border_h;
    uint16_t border_v;
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

#endif // TRAM_SDK_RENDER_SPRITE_H