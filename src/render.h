// TRAMWAY DRIFT AND DUNGEON EXPLORATION SIMULATOR 2022
// All rights reserved.
//
// RENDER.H -- Header for all the graphics stuff
// Definitions of all the things needed for graphics, incl. 3D models,
// effects, UI stuff. If it gets drawn to the screen, it's defined here.

#ifndef RENDER_H
#define RENDER_H

#include <armature.h>

namespace Core::Render {
    class Model;

    extern glm::vec3 CAMERA_POSITION;
    extern glm::quat CAMERA_ROTATION;

    const glm::vec3 CAMERA_FORWARD = glm::vec3(0.0f, 0.0f, -1.0f);
    const glm::vec3 CAMERA_SIDE = glm::vec3(1.0f, 0.0f, 0.0f);

    extern float CAMERA_PITCH;
    extern float CAMERA_YAW;
    const float CAMERA_SENSITIVITY = 0.1f;
    const float CAMERA_SPEED = 0.1f;

    extern float SCREEN_WIDTH;
    extern float SCREEN_HEIGHT;

    extern bool THIRD_PERSON;

    extern float time_of_day;

    enum renderflags: uint32_t {
        FLAG_NO_RENDER = 0,
        FLAG_RENDER = 1,

        FLAG_REVERSE_SORT = 4,
        FLAG_TRANSPARENT = 8,
        FLAG_INTERIOR_LIGHTING = 64
    };

    struct ShaderUniformMatrices {
        glm::mat4 projection;       /// Projection matrix.
        glm::mat4 view;             /// View matrix.
    };

    struct ShaderUniformModelMatrices{
        glm::mat4 model;        /// Model -> world space matrix. Rotates and translates vertices from how they are defined in the model to where they will appear in the world.
        glm::uvec4 modelLights; /// Indices for lights in the light list. The shader will use these 4 indices to determine with which lights the model should be lit up.
        glm::vec4 sunDirection; /// Normalized vector. Sunlight direction.
        glm::vec4 sunColor;     /// Sunlight color.
        glm::vec4 ambientColor; /// Shadow color.
        float time;
        float sunWeight;
    };

    /// Contains pose matrices for shader.
    struct PoseListObject{
        glm::mat4 pose[30];
    };

    /// Contains the minimal information that is needed to draw a Model on screen.
    struct RenderListObject {
        uint32_t flags = 0;

        PoseListObject* pose = nullptr;

        glm::vec3 location;
        glm::quat rotation;

        uint32_t lights[4] = {0};

        uint32_t lightmap = 0;

        uint32_t vbo = 0;
        uint32_t ebo = 0;
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
            uint64_t sortkey;
            sortkey = flags & FLAG_TRANSPARENT ? 1 : 0;
            sortkey = sortkey << 63;
            sortkey = sortkey | ((uint64_t)shader << 48);
            sortkey = sortkey | ((uint64_t)vao << 32);
            sortkey = sortkey | (((uint64_t)(glm::distance(cameraPosition, location) * 0.1f)) & 0x000000000000FFFF);
            return sortkey;
        }
    };

    /// Defines a light.
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
            TEXTURE_WATER,
            FLAT_COLOR
        };
    protected:
        name_t name;                    /// Name of the material. Corresponds to the file name of the material (without the extension).
        uint32_t refCount = 0;          /// How many loaded models are using this resource.
        uint32_t texture = 0;           /// OpenGL name of the texture object.
        Type type = TEXTURE;    /// Used to determine how the texture image should be loaded and which shader to use to draw the material.
        uint32_t width = 0;             /// Width of the texture, in pixels.
        uint32_t height = 0;            /// Height of the texture, in pixels.
        uint8_t channels = 0;           /// Number of color channels.
        uint8_t* textureData = nullptr; /// Pointer to the image data before it's uploaded to video memory.

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
    };

    struct StaticModelVertex{
        glm::vec3 co;
        glm::vec3 normal;
        glm::vec2 tex;
        glm::vec2 lighttex;
        uint32_t texture;
    };

    struct DynamicModelVertex{
        glm::vec3 co;
        glm::vec3 normal;
        glm::vec2 tex;
        glm::ivec4 bone;
        glm::vec4 boneweight;
        uint32_t texture;
    };

    struct LineVertex{
        glm::vec3 co;
        glm::vec3 color;
    };

    struct TextVertex{
        glm::vec2 co;
        glm::vec2 texco;
        glm::vec2 scale;
        glm::vec3 color;
        float thickness;
        uint32_t texture;
    };

    struct ModelIndex{
        glm::ivec3 tri;
    };


    struct ModelData{};

    class Model : public Resource {
    public:
    enum VertexFormat {
            STATIC_VERTEX,
            DYNAMIC_VERTEX,
            LINE_VERTEX,
            TEXT_VERTEX
        };
    protected:
        VertexFormat vertForm;
        name_t name;          /// Name of the model. Doubles as the filename.

        uint32_t vbo = 0;       /// OpenGL vertex buffer name.
        uint32_t ebo = 0;       /// OpenGL element buffer name.
        uint32_t vao = 0;       /// OpenGL vertex array object name.

        uint32_t eboOff[6] = {0};   /// Offset into the element buffer where the material group starts.
        uint32_t eboLen[6] = {0};   /// Length of the material group.
        uint32_t eboMat[6] = {(uint32_t)(-1)}; /// Idk i don't remebrer.

        uint32_t refCount = 0;
        std::vector<Material*> materials;
        std::vector<Bone> armature;
        ModelData* mData = nullptr;

        static Model* error_model;
        static std::unordered_map<uint64_t, Render::Model*> List;

        friend void RenderListObject::FillFromModel(Model* mdl, uint32_t eboIndex);
    public:
        Model (uint64_t mName) {name = mName; status = UNLOADED; /*res_type = RESOURCE_MATERIAL;*/}

        uint32_t GetVBO() {return vbo;}
        uint32_t GetEBO() {return ebo;}
        uint32_t GetVAO() {return vao;}
        bool IsEBOEmpty(uint32_t eboIndex) {return eboLen[eboIndex] == 0;};
        bool Load(){
            LoadFromDisk();
            LoadFromMemory();
            return true;
        };
        bool Unload();
        void LoadFromDisk();
        void LoadFromMemory();
        size_t GetArmatureSize(){return armature.size();};
        Bone* GetArmatureFirstPtr(){return &armature.front();};
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

    class VertexShader;
    class FragmentShader;
    
    class ShaderProgram {
    public:
        VertexShader* vertexShader;
        FragmentShader* fragmentShader;
        uint32_t compiled_shader = 0;
        bool is_compiled = false;

        ShaderProgram (VertexShader* vertexShader, FragmentShader* fragmentShader) : vertexShader(vertexShader), fragmentShader(fragmentShader) {}

        void Link ();
        
        void BindTextures ();
        
        static void BindUniformBlock (bool (*filter)(ShaderProgram&), const char* const name, uint32_t binding);

        static void Add (const ShaderProgram& shader);
        
        static ShaderProgram* Find (Model::VertexFormat vertexType, Material::Type materialType);
        
        static void CompileAll ();
        
    protected:
        static std::vector<ShaderProgram> shaderPrograms;
    };

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

    extern bool DRAW_PHYSICS_DEBUG;
    extern bool DRAW_PATH_DEBUG;

    extern float FRAME_LIMIT;

    void Init();
    void Render();
    void CompileShaders();
    uint32_t FindShader(Model::VertexFormat format, Material::Type type);

    void SetSun(float timeOfDay);

    void AddLine(const glm::vec3& from, const glm::vec3& to, const glm::vec3& color);
    void AddLineMarker(const glm::vec3& location, const glm::vec3& color);
    void SetGlyph(float x, float y, float tex_x, float tex_y, float width, float height, float scale_x, float scale_y, float thickness, uint32_t texture, const glm::vec3& color);

    extern std::vector<LineVertex> colorlines;
    extern std::vector<TextVertex> textvertices;

    extern Pool<PoseListObject> poseList;
    extern Pool<RenderListObject> renderList;

    //extern Queue<Render::Command> rendercommands;

    extern ShaderUniformMatrices matrices;             // from where the view, projection matrices get copied
    extern ShaderUniformModelMatrices modelMatrices;   // from where the model matrix and light indexes get copied
}

#endif // RENDER_H
