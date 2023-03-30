// TRAMWAY DRIFT AND DUNGEON EXPLORATION SIMULATOR 2022
// All rights reserved.

#ifndef RENDER_OPENGL_RENDERER_H
#define RENDER_OPENGL_RENDERER_H

#include <render/renderer.h>

namespace tram::Render::OpenGL {
    void Init();

    void Render();
    
    void CompileShaders();
    
    uint32_t FindShader(vertexformat_t format, materialtype_t type);
    
    void BindUniformBlock (const char* name, uint32_t binding);
    
    void SetScreenSize (float width, float height);
    
    drawlistentry_t InsertDrawListEntry ();
    drawlistentry_t InsertDrawListEntryFromModel (Model* model);
    
    uint32_t GetFlags (drawlistentry_t entry);
    void SetFlags (drawlistentry_t entry, uint32_t flags);
    void SetPose (drawlistentry_t entry, Pose* pose);
    void SetLightmap (drawlistentry_t entry, uint32_t lightmap);
    void SetLights (drawlistentry_t entry, uint32_t* lights);
    void SetLocation (drawlistentry_t entry, glm::vec3& location);
    void SetRotation (drawlistentry_t entry, glm::quat& rotation);
    void SetDrawListVertexArray (drawlistentry_t entry, uint32_t vertex_array_handle);
    void SetDrawListElements (drawlistentry_t entry, uint32_t element_offset, uint32_t element_length);
    void SetDrawListShader (drawlistentry_t entry, vertexformat_t vertex_format, materialtype_t material_type);
    void SetDrawListTextures (drawlistentry_t entry, size_t texture_count, uint32_t* textures);
    
    void RemoveDrawListEntry (drawlistentry_t entry);
    
    uint32_t CreateTexture (ColorMode color_mode, TextureFilter texture_filter, uint32_t width, uint32_t height, void* data);
    
    void CreateIndexedVertexArray (const VertexDefinition& vertex_format, vertexhandle_t& vertex_buffer_handle, vertexhandle_t& element_buffer_handle,  vertexhandle_t& vertex_array_handle, size_t vertex_size, void* vertex_data, size_t index_size, void* index_data);
    void CreateVertexArray (const VertexDefinition& vertex_format, vertexhandle_t& vertex_buffer_handle,  vertexhandle_t& vertex_array_handle);
    void UpdateVertexArray (vertexhandle_t vertex_buffer_handle, size_t data_size, void* data);
    
    void RenderFrame ();
    
    struct DrawListEntry {
        uint32_t flags = 0;

        Pose* pose = nullptr;

        glm::vec3 location = glm::vec3(0.0f, 0.0f, 0.0f);
        glm::quat rotation = glm::quat(glm::vec3(0.0f, 0.0f, 0.0f));

        uint32_t lights[4] = {0};
        
        uint32_t layer = 0;
        uint32_t lightmap = 0;
        uint32_t vao = 0;
        uint32_t eboLen = 0;
        uint32_t eboOff = 0;
        uint32_t shader = 0;
        uint32_t texCount = 0;
        uint32_t textures[15] = {0};
		
		char padding [4];

        /// Copies the element and vertex array names, offsets, etc. from the Model.
        //void FillFromModel(Model* mdl, uint32_t eboIndex);

        /// Assembles a key for sorting
        uint64_t CalcSortKey(const glm::vec3& cameraPosition){
            uint64_t sortkey = 0;
            sortkey = flags & FLAG_TRANSPARENT ? 1 : 0;
            sortkey = sortkey << 60;
            sortkey = sortkey | (((uint64_t)layer) << 61);    // 3 bits for the layer number
            sortkey = sortkey | (((uint64_t)shader) << 48);   // 12 bits for the shader
            sortkey = sortkey | (((uint64_t)vao) << 32);      // 16 bits or the vertex array number
            // TODO: reverse the distance if FLAG_REVERSE_SORT is set
            // also i think that the bitmask for the distance thing is incorrect
            sortkey = sortkey | (((uint64_t)(glm::distance(cameraPosition, location) * 3000000.0f)) & 0x00000000FFFFFFFF); // 32 bits for the distance
            return sortkey;
        }
    };
}

#endif // RENDER_OPENGL_RENDERER_H