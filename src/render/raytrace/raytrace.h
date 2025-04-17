// Tramway Drifting and Dungeon Exploration Simulator SDK Runtime

#ifndef TRAM_SDK_RENDER_RAYTRACE_RAYTRACE_H
#define TRAM_SDK_RENDER_RAYTRACE_RAYTRACE_H

#include <render/render.h>
#include <render/renderer.h>

namespace tram::Render::API {

struct AssemblyLayer {
    vec4 diffuse;
    int texture;
    int sample_x;
    int sample_y;
    float sublayer_opacity;
    vec4 specular;
};

struct AssemblyLayers {
    int width, height;
    std::vector<std::vector<AssemblyLayer>> pixels;
};

void SetMaterialAssemblyIndex(material_t material, int index);
void SetUseAssembly(bool);
AssemblyLayers GetAssemblyLayers();


}

#endif // TRAM_SDK_RENDER_RAYTRACE_RAYTRACE_H