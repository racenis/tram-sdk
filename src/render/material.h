// TRAMWAY DRIFT AND DUNGEON EXPLORATION SIMULATOR 2022
// All rights reserved.

#ifndef RENDER_MATERIAL_H
#define RENDER_MATERIAL_H

#include <render/render.h>

namespace tram::Render {
    class Material : public Resource {
    protected:
        texturehandle_t texture = 0;
        
        materialtype_t type = MATERIAL_TEXTURE;
        uint32_t width = 0;
        uint32_t height = 0;
        uint8_t channels = 0;
        uint8_t* textureData = nullptr;
        size_t approx_vram_usage = 0;
    public:
        Material (){}
        Material(name_t mName, materialtype_t mType) {
            name = mName;
            type = mType;
            status = UNLOADED;
        }
        inline texturehandle_t GetTexture() const { return texture; };
        inline uint32_t GetWidth() const {return width;};
        inline uint32_t GetHeight() const {return height;};
        inline materialtype_t GetType() const {return type;};
        bool Load(){
            LoadFromDisk();
            LoadFromMemory();
            return true;
        }
        bool Unload();
        void LoadFromDisk();
        void LoadFromMemory();
        static Material* Find (name_t name);
        static Material* Make (name_t name, materialtype_t type);
        void MakePattern (vec3 color1, vec3 color2);
        static void LoadMaterialInfo(const char* filename);
        friend class Sprite;
    };
}

#endif // RENDER_MATERIAL_H