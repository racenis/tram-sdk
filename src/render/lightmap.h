// Tramway Drifting and Dungeon Exploration Simulator SDK Runtime

#ifndef TRAM_SDK_RENDER_LIGHTMAP_H
#define TRAM_SDK_RENDER_LIGHTMAP_H

#include <render/render.h>

namespace tram::Render {

enum LightmapType {
    LIGHTMAP_SINGLE,
    LIGHTMAP_CHANNELS
};

class Lightmap : public Resource {
public:
    inline texturearray_t GetTexture() const { return texture; }
    inline uint32_t GetWidth() const { return width; }
    inline uint32_t GetHeight() const { return height; }
    inline LightmapType GetType() const { return type; }
    inline void SetType(LightmapType type) { this->type = type; }

    inline MaterialFilter GetMaterialFilter() const { return filter; }
    inline void SetMaterialFilter(MaterialFilter filter) { this->filter = filter; FlushToAPI(); }

    void LoadFromDisk();
    void LoadFromMemory();
    
    void Unload();
    
    static Lightmap* Find(id_t index);
    static Lightmap* Find(name_t name);
protected:
    Lightmap(name_t name) : Resource(name) {}
    Lightmap(id_t index) : Resource() { this->index = index; }
    
    texturearray_t texture = {};

    LightmapType type = LIGHTMAP_SINGLE;
    MaterialFilter filter = FILTER_LINEAR;

    void FlushToAPI();
    
    id_t index = 0;
    uint32_t width = 0;
    uint32_t height = 0;
    uint8_t* texture_data = nullptr;
    size_t approx_vram_usage = 0;
};

}

#endif // TRAM_SDK_RENDER_LIGHTMAP_H