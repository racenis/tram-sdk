// TRAMWAY DRIFT AND DUNGEON EXPLORATION SIMULATOR 2022
// All rights reserved.

#ifndef RENDER_SPRITE_H
#define RENDER_SPRITE_H

#include <render/render.h>

namespace tram::Render {
    class Material;
    class Sprite : public Resource {
    public:
        struct Frame {
            uint16_t offset_x;
            uint16_t offset_y;
            uint16_t width;
            uint16_t height;
            uint16_t drop;
            uint16_t border;
            float scale;
            float length;       // idk what this is supposed to be
        };
        
        std::vector<Frame> frames;
    protected:
        Material* material = nullptr;
    public:
        Sprite(UID name) : Resource(name) {}
        inline Material* GetMaterial () const { return material; }
        inline void SetMaterial (Material* mat) { material = mat; }
        bool Load(){
            LoadFromDisk();
            LoadFromMemory();
            return true;
        }
        bool Unload();
        void AutogenTiledFrames(uint16_t frame_offset_x, uint16_t frame_offset_y, uint16_t frame_width, uint16_t frame_height, uint16_t frames_per_row, uint16_t frame_count, float scale, float length);
        void LoadFromDisk();
        void LoadFromMemory();
        static Sprite* Find(name_t name);
    };
}

#endif // RENDER_SPRITE_H