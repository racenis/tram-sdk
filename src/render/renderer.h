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
    extern std::vector<SpriteVertex> textvertices;
    extern std::vector<SpriteVertex> glyphvertices;
    
    class GeometryBatch {
    public:
        Material::Type material_type;
        Model::VertexFormat vertex_format;
        
        uint32_t vertex_array;
        uint32_t vertex_buffer;
        
        RenderListObject* render_object = nullptr;
        Material* material_list[16] = {nullptr};
        union {
            std::vector<SpriteVertex>* sprite_vector = nullptr;
            std::vector<LineVertex>* line_vector;
        };
        
        static void Find(uint32_t& index, GeometryBatch*& ptr, Material* mat, Material::Type mat_type, Model::VertexFormat vert_fmt);
        static void Remove(uint32_t index, GeometryBatch* ptr);
        static void Update();
    protected:
        static std::vector<GeometryBatch> geometry_batches;
    };
    
}

namespace Core::Render::OpenGL {
    void Init();

    void Render();
    
    void ScreenSize(float width, float height);
    
    void CompileShaders();
    
    uint32_t FindShader(Model::VertexFormat format, Material::Type type);
    
    void BindUniformBlock (const char* name, uint32_t binding);
    
    
    
    DrawListEntryHandle InsertDrawListEntry();
    DrawListEntryHandle InsertDrawListEntry(Model* model);
    
    uint32_t GetFlags(DrawListEntryHandle entry);
    void SetFlags(DrawListEntryHandle entry, uint32_t flags);
    void SetPose(DrawListEntryHandle entry, PoseListObject* pose);
    void SetLightmap(DrawListEntryHandle entry, uint32_t lightmap);
    void SetLights(DrawListEntryHandle entry, uint32_t* lights);
    void SetLocation(DrawListEntryHandle entry, glm::vec3& location);
    void SetRotation(DrawListEntryHandle entry, glm::quat& rotation);
    
    void RemoveDrawListEntry(DrawListEntryHandle entry);
}

#endif //RENDERER_H