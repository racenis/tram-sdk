// TRAMWAY DRIFT AND DUNGEON EXPLORATION SIMULATOR 2022
// All rights reserved.
//
// RENDER.H -- Simplified rendering interface
// Rendering interface on the level of model, material, sprite abstractions.

#ifndef RENDER_H
#define RENDER_H

#include <render/armature.h>


namespace Core::Render {
    class Model;

    extern glm::vec3 CAMERA_POSITION;
    extern glm::quat CAMERA_ROTATION;
    
    extern glm::vec3 SUN_DIRECTION;
    extern glm::vec3 SUN_COLOR;
    extern glm::vec3 AMBIENT_COLOR;
    
    // shouldn't these be in ui.h?
    extern float CAMERA_PITCH;
    extern float CAMERA_YAW;

    // maybe make these not be public?
    extern float SCREEN_WIDTH;
    extern float SCREEN_HEIGHT;

    extern bool THIRD_PERSON;

    // this should be yeeted
    extern float time_of_day;

    typedef uint32_t vertexhandle_t;
    typedef uint32_t texturehandle_t;
    // this is just a placeholder (or is it?)
    struct DrawListEntryHandle {
        void* draw_list_entries[6] = { nullptr };
    };

    class Material : public Resource {
    public:
        enum Type {
            TEXTURE,
            TEXTURE_ALPHA,
            TEXTURE_LIGHTMAP,
            TEXTURE_MSDF,
            TEXTURE_GLYPH,
            TEXTURE_WATER,
            FLAT_COLOR
        };
    protected:
        texturehandle_t texture = 0;
        
        Type type = TEXTURE;
        uint32_t width = 0;
        uint32_t height = 0;
        uint8_t channels = 0;
        uint8_t* textureData = nullptr;
        size_t approx_vram_usage = 0;

        static Material* error_material;
    public:

        Material (){}
        Material(name_t mName, Material::Type mType) {
            name = mName;
            type = mType;
            status = UNLOADED;
        }
        inline texturehandle_t GetTexture() const { return texture; };
        inline uint32_t GetWidth() const {return width;};
        inline uint32_t GetHeight() const {return height;};
        inline Material::Type GetType() const {return type;};
        bool Load(){
            LoadFromDisk();
            LoadFromMemory();
            return true;
        }
        bool Unload();
        void LoadFromDisk();
        void LoadFromMemory();
        static Material* Find(name_t name);
        static void SetErrorMaterial(Material* mat) { error_material = mat; mat->Load(); }
        static void LoadMaterialInfo(const char* filename);
        friend class Sprite;
    };

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

        static Model* error_model;
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
        static void SetErrorModel(Model* model) { error_model = model; model->Load(); }
    };

    class Sprite : public Resource {
    public:
        struct Frame {
            uint16_t offset_x;
            uint16_t offset_y;
            uint16_t width;
            uint16_t height;
            float scale;
            float length;
        };
        
        std::vector<Frame> frames;
    protected:
        Material* material;
        
        static std::unordered_map<uint64_t, Sprite*> List;
    public:
        Sprite (){}
        Sprite(uint64_t name) : Resource(name) {}
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

    // TODO: move into opengl-specific code
    const uint32_t MAX_MATERIALS_PER_MODEL = 15;
    
    const glm::vec3 COLOR_WHITE(1.0f, 1.0f, 1.0f);
    const glm::vec3 COLOR_RED(1.0f, 0.0f, 0.0f);
    const glm::vec3 COLOR_GREEN(0.0f, 1.0f, 0.0f);
    const glm::vec3 COLOR_BLUE(0.0f, 0.0f, 1.0f);
    const glm::vec3 COLOR_YELLOW(1.0f, 1.0f, 0.0f);
    const glm::vec3 COLOR_PINK(1.0f, 0.0f, 1.0f);
    const glm::vec3 COLOR_CYAN(0.0f, 1.0f, 1.0f);
    const glm::vec3 COLOR_BLACK(0.0f, 0.0f, 0.0f);

    const glm::vec3 COLOR_DISABLED(0.3f, 0.3f, 0.3f);


    extern Material FONT_REGULAR;  // futura knock-off
    extern Material FONT_TITLE;    // helvetica
    extern Material FONT_SYMBOLS;  // ornamentation and other non-text glyphs
    extern Material GLYPH_GUI;
    extern Material GLYPH_TEXT;
    extern Material GLYPH_TEXT_BOLD;
    extern Material GLYPH_HEADERS;

    extern float FRAME_LIMIT;

    void Init();
    void Render();
    void ScreenSize(float width, float height);
    void Project(const glm::vec3& point, glm::vec3& result);

    void SetSun(float timeOfDay);

    void AddLine(const glm::vec3& from, const glm::vec3& to, const glm::vec3& color);
    void AddLineMarker(const glm::vec3& location, const glm::vec3& color);
    void SetGlyph(float x, float y, float tex_x, float tex_y, float width, float height, float scale_x, float scale_y, float thickness, uint32_t texture, const glm::vec3& color);
    void SetGlyph(const float& x, const float& y, const float& w, const float& h, const float& tex_x, const float& tex_y, const float& tex_w, const float& tex_h, const glm::vec3& color, const uint32_t& tex);
}

#endif // RENDER_H
