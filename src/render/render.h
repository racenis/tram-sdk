// TRAMWAY DRIFT AND DUNGEON EXPLORATION SIMULATOR 2022
// All rights reserved.
//
// RENDER.H -- Header for all the graphics stuff
// Definitions of all the things needed for graphics, incl. 3D models,
// effects, UI stuff. If it gets drawn to the screen, it's defined here.

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

    const glm::vec3 CAMERA_FORWARD = glm::vec3(0.0f, 0.0f, -1.0f);
    const glm::vec3 CAMERA_SIDE = glm::vec3(1.0f, 0.0f, 0.0f);
    const glm::vec3 CAMERA_UP = glm::vec3(0.0f, 1.0f, 0.0f);
    

    extern float CAMERA_PITCH;
    extern float CAMERA_YAW;

    extern float SCREEN_WIDTH;
    extern float SCREEN_HEIGHT;

    extern bool THIRD_PERSON;

    extern float time_of_day;

    enum renderflags: uint32_t {
        FLAG_NO_RENDER = 0,
        FLAG_RENDER = 1,

        FLAG_DRAW_INDEXED = 4,
        FLAG_REVERSE_SORT = 8,
        FLAG_TRANSPARENT = 16,
        FLAG_NO_DEPTH_TEST = 32,
        FLAG_INTERIOR_LIGHTING = 64,
        FLAG_DRAW_LINES = 128
    };

    struct RenderListObject {
        uint32_t flags = 0;

        PoseListObject* pose = nullptr;

        glm::vec3 location = glm::vec3(0.0f, 0.0f, 0.0f);
        glm::quat rotation = glm::quat(glm::vec3(0.0f, 0.0f, 0.0f));

        uint32_t lights[4] = {0};
        
        uint32_t layer = 0;
        uint32_t lightmap = 0;
        //uint32_t vbo = 0;
        //uint32_t ebo = 0;
        uint32_t vao = 0;
        uint32_t eboLen = 0;
        uint32_t eboOff = 0;
        uint32_t shader = 0;
        uint32_t texCount = 0;
        uint32_t textures[15] = {0};

        /// Copies the element and vertex array names, offsets, etc. from the Model.
        void FillFromModel(Model* mdl, uint32_t eboIndex);

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
        
    struct VertexProperty {
        enum {
            POSITION_VECTOR,
            NORMAL_VECTOR,
            COLOR_VECTOR,
            TEXTURE_COORDINATE,
            TEXTURE_INDEX,
            LIGHTMAP_COORDINATE,
            BONE_INDEX,
            BONE_WEIGHT,
            OTHER
        } function;
        enum {
            FLOAT32,
            INT32,
            UINT32
        } type;
        uint32_t size;
        uint32_t stride;
        uint64_t offset;
    };
    
    typedef std::vector<VertexProperty> VertexDefinition;

    // TODO: move into opengl-specific code
    struct LightListObject {
        glm::vec3 location = {0.0f, 0.0f, 0.0f};    /// Location of the light in the world.
        float padding;
        glm::vec3 color = {0.0f, 0.0f, 0.0f};       /// Color of the light
        float distance = 0.0f;                      /// How far the light will affect.
        float padding2[8];
    };

    extern Pool<LightListObject> lightPool;    /// A list of all the lights in the world at the moment.
    extern Octree<uint32_t> lightTree;

    /// Finds the 4 nearest lights and puts their indices in a RenderListObject.
    void CalcLights(RenderListObject* obj);

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
        
        struct OpenGL {
            uint32_t texture_name;
        };
        
    protected:
        uint32_t texture = 0;
        
        //name_t name;
        //uint32_t refCount = 0; // wut
        Type type = TEXTURE;
        uint32_t width = 0;
        uint32_t height = 0;
        uint8_t channels = 0;
        uint8_t* textureData = nullptr;
        size_t approx_vram_usage = 0;

        static Material* error_material;
        static std::unordered_map<uint64_t, Material> List;

    public:

        Material (){}
        Material(uint64_t mName, Material::Type mType) {
            name = mName;
            type = mType;
            status = UNLOADED;
            //res_type = RESOURCE_MATERIAL;
        }
        inline uint32_t GetTexture() const {return texture;};
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
        static void SetErrorMaterial(Material* mat) {error_material = mat; mat->Load();}
        static Material* Find(name_t name);
        static void LoadMaterialInfo(const char* filename);
        friend class Sprite;
    };

    struct StaticModelVertex{
        glm::vec3 co;
        glm::vec3 normal;
        glm::vec2 tex;
        glm::vec2 lighttex;
        uint32_t texture;
    };
    
    static const VertexDefinition STATIC_MODEL_VERTEX_DEFINITION = {
        {VertexProperty::POSITION_VECTOR,       VertexProperty::FLOAT32, 3, sizeof(StaticModelVertex), offsetof(StaticModelVertex, co)},
        {VertexProperty::NORMAL_VECTOR,         VertexProperty::FLOAT32, 3, sizeof(StaticModelVertex), offsetof(StaticModelVertex, normal)},
        {VertexProperty::TEXTURE_COORDINATE,    VertexProperty::FLOAT32, 2, sizeof(StaticModelVertex), offsetof(StaticModelVertex, tex)},
        {VertexProperty::LIGHTMAP_COORDINATE,   VertexProperty::FLOAT32, 2, sizeof(StaticModelVertex), offsetof(StaticModelVertex, lighttex)},
        {VertexProperty::TEXTURE_INDEX,         VertexProperty::UINT32,  1, sizeof(StaticModelVertex), offsetof(StaticModelVertex, texture)}
    };

    struct DynamicModelVertex{
        glm::vec3 co;
        glm::vec3 normal;
        glm::vec2 tex;
        glm::ivec4 bone;
        glm::vec4 boneweight;
        uint32_t texture;
    };
    
    static const VertexDefinition DYNAMIC_MODEL_VERTEX_DEFINITION = {
        {VertexProperty::POSITION_VECTOR,       VertexProperty::FLOAT32, 3, sizeof(DynamicModelVertex), offsetof(DynamicModelVertex, co)},
        {VertexProperty::NORMAL_VECTOR,         VertexProperty::FLOAT32, 3, sizeof(DynamicModelVertex), offsetof(DynamicModelVertex, normal)},
        {VertexProperty::TEXTURE_COORDINATE,    VertexProperty::FLOAT32, 2, sizeof(DynamicModelVertex), offsetof(DynamicModelVertex, tex)},
        {VertexProperty::BONE_INDEX,            VertexProperty::UINT32,  4, sizeof(DynamicModelVertex), offsetof(DynamicModelVertex, bone)},
        {VertexProperty::BONE_WEIGHT,           VertexProperty::FLOAT32, 4, sizeof(DynamicModelVertex), offsetof(DynamicModelVertex, boneweight)},
        {VertexProperty::TEXTURE_INDEX,         VertexProperty::UINT32,  1, sizeof(DynamicModelVertex), offsetof(DynamicModelVertex, texture)}
    };
    
    struct SpriteVertex {
        glm::vec3 co;
        glm::vec2 voffset;
        glm::vec2 texco;
        glm::vec3 color;
        float verticality;
        uint32_t texture;
    };

    static const VertexDefinition SPRITE_VERTEX_DEFINITION = {
        {VertexProperty::POSITION_VECTOR,       VertexProperty::FLOAT32, 2, sizeof(SpriteVertex), 0},
        {VertexProperty::TEXTURE_COORDINATE,    VertexProperty::FLOAT32, 2, sizeof(SpriteVertex), offsetof(SpriteVertex, texco)},
        {VertexProperty::COLOR_VECTOR,          VertexProperty::FLOAT32, 3, sizeof(SpriteVertex), offsetof(SpriteVertex, color)},
        
        {VertexProperty::OTHER,                 VertexProperty::FLOAT32, 2, sizeof(SpriteVertex), offsetof(SpriteVertex, voffset)},
        {VertexProperty::OTHER,                 VertexProperty::FLOAT32, 1, sizeof(SpriteVertex), offsetof(SpriteVertex, verticality)},
        
        {VertexProperty::TEXTURE_INDEX,         VertexProperty::UINT32,  1, sizeof(SpriteVertex), offsetof(SpriteVertex, texture)}
    };
    
    struct LineVertex{
        glm::vec3 co;
        glm::vec3 color;
    };
    
    static const VertexDefinition LINE_VERTEX_DEFINITION = {
        {VertexProperty::POSITION_VECTOR,       VertexProperty::FLOAT32, 3, sizeof(LineVertex), 0},
        {VertexProperty::COLOR_VECTOR,          VertexProperty::FLOAT32, 3, sizeof(LineVertex), offsetof(LineVertex, color)}
    };

    struct ModelIndex{
        glm::ivec3 tri;
    };
    
    // this is just a placeholder
    struct DrawListEntryHandle {
        void* draw_list_entries[6] = { nullptr };
    };


    struct ModelData{};

    // TODO: figure out what to do with the models
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
        
        uint32_t vertex_buffer_handle = 0;
        uint32_t element_buffer_handle = 0;
        uint32_t vertex_array_handle = 0;
        
        std::vector<ElementRange> element_ranges;
        
        std::vector<Material*> materials;
        
    protected:
        std::vector<Bone> armature;
        ModelData* mData = nullptr;
        size_t approx_vram_usage = 0;

        static Model* error_model;
        static std::unordered_map<uint64_t, Render::Model*> List;

        friend void RenderListObject::FillFromModel(Model* mdl, uint32_t eboIndex);
    public:
        Model (uint64_t mName) {name = mName; status = UNLOADED; /*res_type = RESOURCE_MATERIAL;*/}

        bool Load(){
            LoadFromDisk();
            LoadFromMemory();
            return true;
        };
        bool Unload();
        void LoadFromDisk();
        void LoadFromMemory();
        size_t GetArmatureSize(){return armature.size();};
        Bone* GetArmature(){return &armature.front();};
        static void SetErrorModel(Model* model) {error_model = model; model->Load();}
        static Model* Find(name_t name);
    };

    struct StaticModelData : public ModelData{
        std::vector<StaticModelVertex> vertices;
        std::vector<ModelIndex> indices;
    };

    struct DynamicModelData : public ModelData{
        std::vector<DynamicModelVertex> vertices;
        std::vector<ModelIndex> indices;
        std::vector<unsigned int> groups;
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
        
        static std::unordered_map<name_t, Sprite*> List;
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

    //extern std::vector<LineVertex> colorlines;
    //extern std::vector<TextVertex> textvertices;

    
    //extern Pool<RenderListObject> renderList;

}

#endif // RENDER_H
