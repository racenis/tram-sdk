// Tramway Drifting and Dungeon Exploration Simulator SDK Runtime

#ifndef TRAM_SDK_RENDER_ENVIRONMENT_H
#define TRAM_SDK_RENDER_ENVIRONMENT_H

#include <render/render.h>

namespace tram::Render {

enum EnvironmentType {
    ENVIRONMENT_SPHERE,
};

class Environment : public Resource {
public:
    inline texturearray_t GetTexture() const { return texture; }
    inline uint32_t GetWidth() const { return width; }
    inline uint32_t GetHeight() const { return height; }
    inline EnvironmentType GetType() const { return type; }

    inline void SetType(EnvironmentType type) { this->type = type; }

    void LoadFromDisk();
    void LoadFromMemory();
    
    void Unload();
    
    static Environment* Find(name_t graph, uint32_t index);
    static Environment* Find(name_t name);
protected:
    Environment(name_t name) : Resource(name) {}
    Environment(name_t graph, uint32_t index) : Resource() {}
    
    texturearray_t texture = {};

    EnvironmentType type = ENVIRONMENT_SPHERE;
    
    name_t graph;
    uint32_t index = 0;
    
    uint32_t width = 0;
    uint32_t height = 0;
    uint8_t* texture_data = nullptr;
    size_t approx_vram_usage = 0;
};

}

#endif // TRAM_SDK_RENDER_ENVIRONMENT_H