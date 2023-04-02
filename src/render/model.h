// TRAMWAY DRIFT AND DUNGEON EXPLORATION SIMULATOR 2022
// All rights reserved.

#ifndef RENDER_MODEL_H
#define RENDER_MODEL_H

#include <render/material.h>

namespace tram::Render{
    struct ModelData;

    class Model : public Resource {
    public:        
        struct ElementRange {
            uint32_t element_offset = 0;
            uint32_t element_length = 0;
            uint32_t material_count = 0;
            materialtype_t material_type;
            uint32_t materials[15] = { 0 };
        };
        
        vertexformat_t vertex_format = VERTEX_STATIC;
        
        vertexhandle_t vertex_buffer_handle = 0;
        vertexhandle_t element_buffer_handle = 0;
        vertexhandle_t vertex_array_handle = 0;
        
        std::vector<ElementRange> element_ranges;
        
        std::vector<Material*> materials;
        
    protected:
        std::vector<Bone> armature;
        ModelData* model_data = nullptr;
        size_t approx_vram_usage = 0;
    public:
        Model (name_t name) : Resource(name) {}

        void LoadFromDisk();
        void LoadFromMemory();
        
        void Unload() {}
        
        size_t GetArmatureSize() { return armature.size(); }
        Bone* GetArmature() { return &armature.front(); }
        
        static Model* Find(name_t name);
    };
}


#endif // RENDER_MODEL_H