// TRAMWAY DRIFT AND DUNGEON EXPLORATION SIMULATOR 2022
// All rights reserved.
//
// RENDERER.H -- Renderer
// Functions that all renderer backends need to implement.

#ifndef RENDERER_H
#define RENDERER_H

#include <render/render.h>

namespace Core::Render {
    extern Pool<RenderListObject> renderList;
    extern Pool<LightListObject> lightPool;
    extern Octree<uint32_t> lightTree;

    extern std::vector<LineVertex> colorlines;
    extern std::vector<TextVertex> textvertices;
    extern std::vector<GlyphVertex> glyphvertices;
}

namespace Core::Render::OpenGL {
    void Init();

    void Render();
    
    void ScreenSize(float width, float height);
    
    void CompileShaders();
    
    uint32_t FindShader(Model::VertexFormat format, Material::Type type);
    
    void BindUniformBlock (const char* name, uint32_t binding);
    
}

#endif //RENDERER_H