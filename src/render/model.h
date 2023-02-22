// TRAMWAY DRIFT AND DUNGEON EXPLORATION SIMULATOR 2022
// All rights reserved.

#ifndef RENDER_MODEL_H
#define RENDER_MODEL_H

#include <render/material.h>

namespace Core::Render{
    struct ModelData{};

    class Model : public Resource {
    public:
        enum VertexFormat {
            STATIC_VERTEX,
            DYNAMIC_VERTEX,
            SPRITE_VERTEX,
            LINE_VERTEX
        };
        
        struct ElementRange {
            uint32_t element_offset = 0;
            uint32_t element_length = 0;
            uint32_t material_count = 0;
            Material::Type material_type;
            uint32_t materials[15] = { 0 };
        };
        
        VertexFormat vertex_format = STATIC_VERTEX;
        
        vertexhandle_t vertex_buffer_handle = 0;
        vertexhandle_t element_buffer_handle = 0;
        vertexhandle_t vertex_array_handle = 0;
        
        std::vector<ElementRange> element_ranges;
        
        std::vector<Material*> materials;
        
    protected:
        std::vector<Bone> armature;
        ModelData* mData = nullptr;
        size_t approx_vram_usage = 0;
    public:
        Model (name_t mName) {name = mName; status = UNLOADED; /*res_type = RESOURCE_MATERIAL;*/}

        bool Load(){
            LoadFromDisk();
            LoadFromMemory();
            return true;
        };
        bool Unload();
        void LoadFromDisk();
        void LoadFromMemory();
        size_t GetArmatureSize() { return armature.size(); }
        Bone* GetArmature() { return &armature.front(); }
        static Model* Find(name_t name);
        static void LoadErrorModel();
    };
}


#endif // RENDER_MODEL_H