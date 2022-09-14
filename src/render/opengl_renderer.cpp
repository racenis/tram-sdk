#include <render/renderer.h>

// too too
#include <glad.c>
//#include <glad_gles3.c>

#include <glm/gtc/type_ptr.hpp>

#include <components/spritecomponent.h>
#include <components/particlecomponent.h>

namespace Core::Render::OpenGL {
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
        float screenWidth;
        float screenHeight;
    };
    
    ShaderUniformMatrices matrices;
    ShaderUniformModelMatrices modelMatrices;
    
    class ShaderBuffer {};
    
    const uint32_t matrix_uniform_binding = 0;
    const uint32_t model_matrix_uniform_binding = 1;
    const uint32_t light_uniform_binding = 2;
    const uint32_t bone_uniform_binding = 3;
    
    uint32_t matrix_uniform_buffer;
    uint32_t model_matrix_uniform_buffer;
    uint32_t light_uniform_buffer;
    uint32_t bone_uniform_buffer;
    
    uint32_t MakeUniformBuffer (const char* name, uint32_t binding, uint32_t initial_size) {
        uint32_t handle;
        
        glGenBuffers(1, &handle);
        glBindBuffer(GL_UNIFORM_BUFFER, handle);
        glBufferData(GL_UNIFORM_BUFFER, initial_size, NULL, GL_DYNAMIC_DRAW);

        glBindBufferBase(GL_UNIFORM_BUFFER, binding, handle);

        BindUniformBlock (name, binding);

        glBindBuffer(GL_UNIFORM_BUFFER, 0);
        return handle;
    }
    
    void UploadUniformBuffer (uint32_t handle, uint32_t data_size, void* data) {
        glBindBuffer(GL_UNIFORM_BUFFER, handle);
        glBufferSubData(GL_UNIFORM_BUFFER, 0, data_size, data);
        glBindBuffer(GL_UNIFORM_BUFFER, 0);
    }
    
    struct VertexFormat {
        int32_t size;
        uint32_t type;
        int32_t stride;
        void* pointer;
    };
    
    struct VertexArray {
        uint32_t array_handle;
        uint32_t buffer_handle;
        uint32_t vertex_count;
        bool dynamic_draw;
    };
    
    VertexArray line_array;
    VertexArray text_array;
    VertexArray glyph_array;
    
    VertexArray MakeVertexArray (const std::vector<VertexFormat>& vertex_format, uint32_t initial_size, bool dynamic_draw = true) {
        VertexArray array = { .dynamic_draw = dynamic_draw };
        
        glGenBuffers(1, &array.buffer_handle);
        glBindBuffer(GL_ARRAY_BUFFER, array.buffer_handle);
        glBufferData(GL_ARRAY_BUFFER, initial_size, NULL, dynamic_draw ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW);

        glGenVertexArrays(1, &array.array_handle);

        glBindVertexArray(array.array_handle);

        for (uint32_t i = 0; i < vertex_format.size(); i++){
            auto& a = vertex_format[i];
            
            if (a.type == GL_UNSIGNED_INT) {
                glVertexAttribIPointer(i, a.size, GL_UNSIGNED_INT, a.stride, a.pointer);
            } else {
                glVertexAttribPointer(i, a.size, a.type, GL_FALSE, a.stride, a.pointer);
            }
            
            glEnableVertexAttribArray(i);
        }

        glBindBuffer(GL_ARRAY_BUFFER, array.buffer_handle);

        glBindVertexArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        
        return array;
    }
    
    void UploadVertexArray (VertexArray& array, uint32_t data_size, uint32_t vertex_count, void* data) {
        glBindVertexArray(array.array_handle);
        glBindBuffer(GL_ARRAY_BUFFER, array.buffer_handle);
        glBufferData(GL_ARRAY_BUFFER, data_size, data, array.dynamic_draw ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW);
        array.vertex_count = vertex_count;
    }
    
    void DrawVertexArray (VertexArray& array, const std::vector<Material*>& materials, bool draw_triangles = true) {
        glBindVertexArray(array.array_handle);
        for (unsigned int i = 0; i < materials.size(); i++){
            glActiveTexture(GL_TEXTURE0 + i);
            glBindTexture(GL_TEXTURE_2D, materials[i]->GetTexture());
        }
        
        glDrawArrays(draw_triangles ? GL_TRIANGLES : GL_LINES, 0, array.vertex_count);

        glBindVertexArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }
    
    /*class UniformBuffer : ShaderBuffer {
        uint32_t handle;
        uint32_t binding;
        std::string name;
        
    public:
        size_t data_size;
        void* data;
        
        UniformBuffer (uint32_t binding, std::string name, size_t data_size, void* data) : binding(binding), name(name), data_size(data_size), data(data) {
            
        }
        
        void Init () {
            glGenBuffers(1, &handle);
            glBindBuffer(GL_UNIFORM_BUFFER, handle);
            glBufferData(GL_UNIFORM_BUFFER, data_size, NULL, GL_DYNAMIC_DRAW);

            glBindBufferBase(GL_UNIFORM_BUFFER, binding, handle);

            // TODO: figure out how to automatically create this lambda
            BindUniformBlock ((char*)name.c_str(), binding);
            //ShaderProgram::BindUniformBlock([](ShaderProgram& shader){ return true; }, name.c_str(), binding);

            glBindBuffer(GL_UNIFORM_BUFFER, 0);
        }
        
        void Upload () {
            glBindBuffer(GL_UNIFORM_BUFFER, handle);
            glBufferSubData(GL_UNIFORM_BUFFER, 0, data_size, data);
            glBindBuffer(GL_UNIFORM_BUFFER, 0);
        }
    };*/
    
    class VertexBuffer : ShaderBuffer {
    public:
        struct VertexFormat {
            int32_t size;
            uint32_t type;
            int32_t stride;
            void* pointer;
        };
        
        std::vector<VertexFormat> attributes;
        std::vector<Material*> textures;
        
        uint32_t buffer_handle;
        uint32_t array_handle;
        uint32_t draw_mode;
        size_t vertex_count;
        size_t data_size;
        void* data;
        
        VertexBuffer(const std::vector<VertexFormat>& attributes, const std::vector<Material*>& textures, uint32_t draw_mode, size_t vertex_count, size_t data_size, void* data) : attributes(attributes), textures(textures), draw_mode(draw_mode), vertex_count(vertex_count), data_size(data_size), data(data){
            
        }
        
        void Init () {
            glGenBuffers(1, &buffer_handle);
            glBindBuffer(GL_ARRAY_BUFFER, buffer_handle);
            glBufferData(GL_ARRAY_BUFFER, colorlines.size() * sizeof(LineVertex), &colorlines[0], GL_DYNAMIC_DRAW);

            glGenVertexArrays(1, &array_handle);

            glBindVertexArray(array_handle);

            for (uint32_t i = 0; i < attributes.size(); i++){
                auto& a = attributes[i];
                
                if (a.type == GL_UNSIGNED_INT) {
                    glVertexAttribIPointer(i, a.size, GL_UNSIGNED_INT, a.stride, a.pointer);
                } else {
                    glVertexAttribPointer(i, a.size, a.type, GL_FALSE, a.stride, a.pointer);
                }
                
                glEnableVertexAttribArray(i);
            }

            glBindBuffer(GL_ARRAY_BUFFER, buffer_handle);

            glBindVertexArray(0);
            glBindBuffer(GL_ARRAY_BUFFER, 0);
        }
        
        void UpdateData (size_t vertex_count, size_t data_size, void* data) {
            this->vertex_count = vertex_count;
            this->data_size = data_size;
            this->data = data;
        }
        
        void Upload () {
            glBindVertexArray(array_handle);
            glBindBuffer(GL_ARRAY_BUFFER, buffer_handle);
            glBufferData(GL_ARRAY_BUFFER, data_size, data, GL_DYNAMIC_DRAW);
        }
        
        void Draw () {
            // if this method isn't called right after Upload(), then it will not work
            // hee hee
            for (unsigned int i = 0; i < textures.size(); i++){
                glActiveTexture(GL_TEXTURE0 + i);
                glBindTexture(GL_TEXTURE_2D, textures[i]->GetTexture());
            }
            
            glDrawArrays(draw_mode, 0, vertex_count);

            glBindVertexArray(0);
            glBindBuffer(GL_ARRAY_BUFFER, 0);
        }
        
    };
    


    //auto lineBuffer = VertexBuffer(std::vector<VertexBuffer::VertexFormat>{
    //        {3, GL_FLOAT, sizeof(LineVertex), nullptr},
     //       {3, GL_FLOAT, sizeof(LineVertex), (void*)offsetof(LineVertex, color)}
     //       }, std::vector<Material*>(), GL_LINES, colorlines.size(), colorlines.size() * sizeof(LineVertex), &colorlines[0]);
            
    auto textBuffer = VertexBuffer(std::vector<VertexBuffer::VertexFormat>{
            {2, GL_FLOAT, sizeof(TextVertex), nullptr},
            {2, GL_FLOAT, sizeof(TextVertex), (void*)offsetof(TextVertex, texco)},
            {3, GL_FLOAT, sizeof(TextVertex), (void*)offsetof(TextVertex, color)},
            {2, GL_FLOAT, sizeof(TextVertex), (void*)offsetof(TextVertex, scale)},
            {1, GL_FLOAT, sizeof(TextVertex), (void*)offsetof(TextVertex, thickness)},
            {1, GL_UNSIGNED_INT, sizeof(TextVertex), (void*)offsetof(TextVertex, texture)}
        }, std::vector<Material*>{
            &FONT_REGULAR,
            &FONT_TITLE,
            &FONT_TITLE, // <--- just a placeholder
            &FONT_TITLE, // <--- also a placeholder
            &FONT_SYMBOLS
        }, GL_TRIANGLES, textvertices.size(), textvertices.size() * sizeof(TextVertex), &textvertices[0]);
        
    auto glyphBuffer = VertexBuffer(std::vector<VertexBuffer::VertexFormat>{
        {2, GL_FLOAT, sizeof(GlyphVertex), nullptr},
        {2, GL_FLOAT, sizeof(GlyphVertex), (void*)offsetof(GlyphVertex, texco)},
        {3, GL_FLOAT, sizeof(GlyphVertex), (void*)offsetof(GlyphVertex, color)},
        {1, GL_UNSIGNED_INT, sizeof(GlyphVertex), (void*)offsetof(GlyphVertex, texture)}
    }, std::vector<Material*>{
        &GLYPH_GUI,
        &GLYPH_TEXT,
        &GLYPH_TEXT_BOLD,
        &GLYPH_HEADERS
    }, GL_TRIANGLES, glyphvertices.size(), glyphvertices.size() * sizeof(GlyphVertex), &glyphvertices[0]);
        
        
    void ScreenSize(float width, float height) {
        matrices.projection = glm::perspective(glm::radians(45.0f), SCREEN_WIDTH / SCREEN_HEIGHT, 0.1f, 1000.0f);
    }
    

        
    void Init(){
            
        CompileShaders();

        //lineBuffer.Init();            
        line_array = MakeVertexArray(std::vector<VertexFormat> {
            {3, GL_FLOAT, sizeof(LineVertex), nullptr},
            {3, GL_FLOAT, sizeof(LineVertex), (void*)offsetof(LineVertex, color)}
            }, 0, true);
        
        
        textBuffer.Init();
        glyphBuffer.Init();
        
        //lightUniform.Init();
        //matricesUniform.Init();
        //modelMatricesUniform.Init();
        //boneUniform.Init();
        
        //auto lightUniform = UniformBuffer(2, "Lights", sizeof(LightListObject)*50, lightPool.begin().ptr);
        light_uniform_buffer = MakeUniformBuffer("Lights", light_uniform_binding, sizeof(LightListObject)*50);
        
        //auto matricesUniform = UniformBuffer(0, "Matrices", sizeof(ShaderUniformMatrices), &matrices);
        matrix_uniform_buffer = MakeUniformBuffer("Matrices", matrix_uniform_binding, sizeof(ShaderUniformMatrices));
        
        //auto modelMatricesUniform = UniformBuffer(1, "ModelMatrices", sizeof(ShaderUniformModelMatrices), &modelMatrices);
        model_matrix_uniform_buffer = MakeUniformBuffer("ModelMatrices", model_matrix_uniform_binding, sizeof(ShaderUniformModelMatrices));
        
        //auto boneUniform = UniformBuffer(3, "Bones", sizeof(PoseListObject), nullptr);
        bone_uniform_buffer = MakeUniformBuffer("Bones", bone_uniform_binding, sizeof(PoseListObject));
        
        
        

        matrices.projection = glm::perspective(glm::radians(45.0f), SCREEN_WIDTH / SCREEN_HEIGHT, 0.1f, 1000.0f);

        poseList.AddNew();
        for (size_t i = 0; i < 30; i++){
            poseList.begin()->pose[i] = glm::mat4(1.0f);
        }
        
    }
    
    void Render() {
        //glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClearColor(255.0f/255.0f, 182.0f/255.0f, 193.0f/255.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        modelMatrices.time = FRAME_TIME;
        modelMatrices.sunDirection = glm::vec4(SUN_DIRECTION, 1.0f);
        modelMatrices.sunColor = glm::vec4(SUN_COLOR, 1.0f);
        modelMatrices.ambientColor = glm::vec4(AMBIENT_COLOR, 1.0f);
        modelMatrices.screenWidth = SCREEN_WIDTH;
        modelMatrices.screenHeight = SCREEN_HEIGHT;
        
        matrices.view = glm::inverse(glm::translate(glm::mat4(1.0f), CAMERA_POSITION) * glm::toMat4(CAMERA_ROTATION));

        if (THIRD_PERSON) matrices.view = glm::translate(matrices.view, CAMERA_ROTATION * glm::vec3(0.0f, 0.0f, -20.0f));

        //matricesUniform.Upload();
        UploadUniformBuffer(matrix_uniform_buffer, sizeof(ShaderUniformMatrices), &matrices);

        //lightUniform.Upload();
        UploadUniformBuffer(light_uniform_binding, sizeof(LightListObject)*50, lightPool.begin().ptr);

        static std::vector<std::pair<uint64_t, RenderListObject*>> rvec;

        rvec.clear();

        RenderListObject* robj = renderList.GetFirst();
        RenderListObject* rlast = renderList.GetLast();

        Stats::Start(Stats::RENDER);
        for(;robj < rlast; robj++){
            if(*((uint64_t*)robj) == 0) continue;

            // TODO: do view culling in here

            rvec.push_back(std::pair<uint64_t, RenderListObject*>(robj->CalcSortKey(CAMERA_POSITION), robj));
        }

        sort(rvec.begin(), rvec.end());

        for (std::pair<uint64_t, RenderListObject*>& pp : rvec){
            RenderListObject* robj = pp.second;
            //char debug_text_buffer[200];
            //sprintf(debug_text_buffer, "Robj %d %f %f %f lights %d %d %d %d", robj->vao, robj->location[0], robj->location[1], robj->location[2], robj->lights[0], robj->lights[1], robj->lights[2], robj->lights[3]);
            //GUI::DebugText(debug_text_buffer, robj->location, COLOR_GREEN);

            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, glm::vec3(robj->location[0], robj->location[1], robj->location[2]));

            model *= glm::toMat4(robj->rotation);

            glUseProgram(robj->shader);

            if(robj->pose != nullptr){
                UploadUniformBuffer(bone_uniform_buffer, sizeof(PoseListObject), glm::value_ptr(robj->pose->pose[0]));
                //boneUniform.data = glm::value_ptr(robj->pose->pose[0]);
                //boneUniform.Upload();
            }


            modelMatrices.modelLights.x = robj->lights[0];
            modelMatrices.modelLights.y = robj->lights[1];
            modelMatrices.modelLights.z = robj->lights[2];
            modelMatrices.modelLights.w = robj->lights[3];

            if (robj->flags & FLAG_INTERIOR_LIGHTING)
                modelMatrices.sunWeight = 0.0f;
            else
                modelMatrices.sunWeight = 1.0f;


            modelMatrices.model = model;
            //modelMatricesUniform.Upload();
            UploadUniformBuffer(model_matrix_uniform_buffer, sizeof(ShaderUniformModelMatrices), &modelMatrices);



            for (unsigned int j = 0; j < robj->texCount; j++){
                glActiveTexture(GL_TEXTURE0 + j);
                glBindTexture(GL_TEXTURE_2D, robj->textures[j]);
            }

            if(robj->lightmap != 0){
                glActiveTexture(GL_TEXTURE15);
                glBindTexture(GL_TEXTURE_2D, robj->lightmap);
            }


            if (robj->ebo != 0) {
                glBindVertexArray(robj->vao);
                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, robj->ebo);
                glDrawElements(GL_TRIANGLES, robj->eboLen * 3, GL_UNSIGNED_INT, (void*)(robj->eboOff * 3 * sizeof(uint32_t)));
            } else {
                glBindVertexArray(robj->vao);
                glDrawArrays(GL_TRIANGLES, 0, robj->eboLen);
            }



        }

        // set opengl to draw everything on top and as a line
        glDisable(GL_DEPTH_TEST);
        //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

        //glUseProgram(ShaderProgram::Find(Model::LINE_VERTEX, Material::FLAT_COLOR)->compiled_shader);
        glUseProgram(FindShader(Model::LINE_VERTEX, Material::FLAT_COLOR));
        

        // upload the line buffer and draw lines
        //lineBuffer.UpdateData(colorlines.size(), colorlines.size() * sizeof(LineVertex), &colorlines[0]);
        UploadVertexArray(line_array, colorlines.size() * sizeof(LineVertex), colorlines.size(), &colorlines[0]);
        //lineBuffer.Upload();
        //lineBuffer.Draw();
        DrawVertexArray(line_array, std::vector<Material*>(), false);

        // back to drawing triangles
        //glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        // set special text shader
        //glUseProgram(ShaderProgram::Find(Model::TEXT_VERTEX, Material::TEXTURE_MSDF)->compiled_shader);
        glUseProgram(FindShader(Model::TEXT_VERTEX, Material::TEXTURE_MSDF));

        // upload the text array and draw the text
        textBuffer.UpdateData(textvertices.size(), textvertices.size() * sizeof(TextVertex), &textvertices[0]);
        textBuffer.Upload();
        textBuffer.Draw();
        
        //glUseProgram(ShaderProgram::Find(Model::GLYPH_VERTEX, Material::TEXTURE_GLYPH)->compiled_shader);
        glUseProgram(FindShader(Model::GLYPH_VERTEX, Material::TEXTURE_GLYPH));
        glyphBuffer.UpdateData(glyphvertices.size(), glyphvertices.size() * sizeof(GlyphVertex), &glyphvertices[0]);
        glyphBuffer.Upload();
        glyphBuffer.Draw();

        // back to drawing everything in its supposed place
        glEnable(GL_DEPTH_TEST);

        Stats::Stop(Stats::RENDER);

        // clear the buffers for the next frame
        colorlines.clear();
        textvertices.clear();
        glyphvertices.clear();

        Stats::Stop(Stats::FRAME_NO_SWAP);
    }
   
}

    void Core::Render::Project(const glm::vec3& point, glm::vec3& result) {
        result = glm::project(point, OpenGL::matrices.view, OpenGL::matrices.projection, glm::vec4 (0.0f, 0.0f, Render::SCREEN_WIDTH, Render::SCREEN_HEIGHT));
        result.y = Render::SCREEN_HEIGHT - result.y;
    }

namespace Core {
    using namespace Core::Render::OpenGL;
    
    void SpriteComponent::Uninit(){
        is_ready = true;

        Render::renderList.Remove(robject);
    };

    void SpriteComponent::Start(){
        if(is_ready) return;


        robject = Render::renderList.AddNew();

        glGenBuffers(1, &robject->vbo);
        glBindBuffer(GL_ARRAY_BUFFER, robject->vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(Render::SpriteVertex)*6, NULL, GL_DYNAMIC_DRAW);

        glGenVertexArrays(1, &robject->vao);
        glBindVertexArray(robject->vao);
        
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Render::SpriteVertex), nullptr);
        glEnableVertexAttribArray(0);

        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Render::SpriteVertex), (void*)offsetof(Render::SpriteVertex, voffset));
        glEnableVertexAttribArray(1);
        
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Render::SpriteVertex), (void*)offsetof(Render::SpriteVertex, tex));
        glEnableVertexAttribArray(2);

        glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, sizeof(Render::SpriteVertex), (void*)offsetof(Render::SpriteVertex, verticality));
        glEnableVertexAttribArray(3);

        glVertexAttribIPointer(4, 1, GL_UNSIGNED_INT, sizeof(Render::SpriteVertex), (void*)offsetof(Render::SpriteVertex, texture));
        glEnableVertexAttribArray(4);
        
        glBindBuffer(GL_ARRAY_BUFFER, robject->vbo);

        glBindVertexArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        
        robject->flags = Render::FLAG_RENDER;
        robject->lightmap = 0;
        robject->ebo = 0;
        robject->eboLen = 6;
        robject->eboOff = 0;
        robject->texCount = 1;
        robject->textures[0] = sprite->GetMaterial()->GetTexture();
        robject->lights[0] = 0;
        robject->lights[1] = 0;
        robject->lights[2] = 0;
        robject->lights[3] = 0;
        robject->shader = FindShader(Render::Model::SPRITE_VERTEX, Render::Material::TEXTURE_ALPHA);
        
        is_ready = true;
        UpdateRenderListObject();
    }
    
    void SpriteComponent::Update(){
        if (!is_ready) return;
        
        if (anim_isplaying) {
            UpdateRenderListObject();
        
            if (!(anim_bframe < anim_speed)) {
                anim_bframe = 0;
                anim_frame++;
                if (!(anim_frame < sprite->frames)) anim_frame = 0;
            }
            anim_bframe++;
        }
    }

    void SpriteComponent::UpdateRenderListObject(){
        if (!is_ready) return;
        
        float tex_width = sprite->width;
        float tex_height = sprite->height;
        float tex_w_off = (float)(anim_frame % sprite->frames_w) * tex_width;
        float tex_h_off = (float)(anim_frame / sprite->frames_w) * tex_height;
        float half_width = sprite->width * sprite->scale / 2.0f;
        float half_height = sprite->height * sprite->scale / 2.0f;

        Render::SpriteVertex top_left {
            .co = glm::vec3(0.0f, 0.0f, 0.0f),
            .voffset = glm::vec2 (-half_width, half_height),
            .tex = glm::vec2 (0.0f + tex_w_off, 1.0f - tex_h_off),
            .verticality = 1.0f,
            .texture = 0
        };
        
        Render::SpriteVertex top_right {
            .co = glm::vec3(0.0f, 0.0f, 0.0f),
            .voffset = glm::vec2 (half_width, half_height),
            .tex = glm::vec2 (tex_width + tex_w_off, 1.0f - tex_h_off),
            .verticality = 1.0f,
            .texture = 0
        };
        
        Render::SpriteVertex bottom_left {
            .co = glm::vec3(0.0f, 0.0f, 0.0f),
            .voffset = glm::vec2 (-half_width, -half_height),
            .tex = glm::vec2 (0.0f + tex_w_off, 1.0f - tex_height - tex_h_off),
            .verticality = 1.0f,
            .texture = 0
        };
        
        Render::SpriteVertex bottom_right {
            .co = glm::vec3(0.0f, 0.0f, 0.0f),
            .voffset = glm::vec2 (half_width, -half_height),
            .tex = glm::vec2 (tex_width + tex_w_off, 1.0f - tex_height - tex_h_off),
            .verticality = 1.0f,
            .texture = 0
        };

        std::vector<Render::SpriteVertex> vertices;
        
        vertices.push_back(top_left);
        vertices.push_back(bottom_left);
        vertices.push_back(top_right);
        vertices.push_back(top_right);
        vertices.push_back(bottom_left);
        vertices.push_back(bottom_right);

        glBindBuffer(GL_ARRAY_BUFFER, robject->vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(Render::SpriteVertex)*vertices.size(), &vertices[0], GL_DYNAMIC_DRAW);
        
        
        robject->location = location;
        robject->rotation = glm::quat(glm::vec3(0.0f, 0.0f, 0.0f));
    }
    
            
    void SpriteComponent::Play() {
        anim_isplaying = true;
    }
    
    void SpriteComponent::Pause() {
        anim_isplaying = true;
    }
    
    void SpriteComponent::SetPlaySpeed(size_t speed) {
        anim_speed = speed;
    }
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    void ParticleComponent::Uninit(){
        is_ready = true;

        Render::renderList.Remove(robject);
    };

    void ParticleComponent::Start(){
        if(is_ready) return;


        robject = Render::renderList.AddNew();

        glGenBuffers(1, &robject->vbo);
        glBindBuffer(GL_ARRAY_BUFFER, robject->vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(Render::SpriteVertex)*6, NULL, GL_DYNAMIC_DRAW);

        glGenVertexArrays(1, &robject->vao);
        glBindVertexArray(robject->vao);
        
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Render::SpriteVertex), nullptr);
        glEnableVertexAttribArray(0);

        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Render::SpriteVertex), (void*)offsetof(Render::SpriteVertex, voffset));
        glEnableVertexAttribArray(1);
        
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Render::SpriteVertex), (void*)offsetof(Render::SpriteVertex, tex));
        glEnableVertexAttribArray(2);

        glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, sizeof(Render::SpriteVertex), (void*)offsetof(Render::SpriteVertex, verticality));
        glEnableVertexAttribArray(3);

        glVertexAttribIPointer(4, 1, GL_UNSIGNED_INT, sizeof(Render::SpriteVertex), (void*)offsetof(Render::SpriteVertex, texture));
        glEnableVertexAttribArray(4);
        
        glBindBuffer(GL_ARRAY_BUFFER, robject->vbo);

        glBindVertexArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        
        robject->flags = Render::FLAG_RENDER;
        robject->lightmap = 0;
        robject->ebo = 0;
        robject->eboLen = 6;
        robject->eboOff = 0;
        robject->texCount = 1;
        robject->textures[0] = sprite->GetMaterial()->GetTexture();
        robject->lights[0] = 0;
        robject->lights[1] = 0;
        robject->lights[2] = 0;
        robject->lights[3] = 0;
        robject->shader = FindShader(Render::Model::SPRITE_VERTEX, Render::Material::TEXTURE_ALPHA);
        
        is_ready = true;
        UpdateRenderListObject();
    }
    
    void ParticleComponent::Update(){
        if (!is_ready) return;
        
        for (auto it = particles.begin(); it < particles.end();) {
            it->velocity += gravity;
            it->coords += it->velocity;
            it->age++;
            
            if (it->age > particle_max_age) {
                it = particles.erase(it);
            } else {
                it++;
            }
        }
        
        glm::vec3 random_vec;
        static int tick = 0;
        tick++; srand(tick*6217);
        random_vec.x = 0.5f - ((float)rand()/(float)RAND_MAX);
        tick++; srand(tick*6217);
        random_vec.y = 0.5f - ((float)rand()/(float)RAND_MAX);
        tick++; srand(tick*6217);
        random_vec.z = 0.5f - ((float)rand()/(float)RAND_MAX);
        
        glm::vec3 velocity = initial_velocity + (random_vec*initial_velocity_randomness);
        particles.push_back(Particle {velocity, velocity, 0});
        
        //std::cout << random_vec.x << "\t" << random_vec.y << "\t" << random_vec.z << std::endl;
        //std::cout << "particles: " << particles.size() << std::endl;
        
        UpdateRenderListObject();
    }

    void ParticleComponent::UpdateRenderListObject(){
        if (!is_ready) return;
        
        float tex_width = sprite->width;
        float tex_height = sprite->height;
        float tex_w_off = 0.0f;//(float)(anim_frame % sprite->frames_w) * tex_width;
        float tex_h_off = 0.0f;//(float)(anim_frame / sprite->frames_w) * tex_height;
        float half_width = sprite->width * sprite->scale / 2.0f;
        float half_height = sprite->height * sprite->scale / 2.0f;

        std::vector<Render::SpriteVertex> vertices;

        for (auto particle : particles) {
            Render::SpriteVertex top_left {
                //.co = glm::vec3(0.0f, 0.0f, 0.0f),
                .co = particle.coords,
                .voffset = glm::vec2 (-half_width, half_height),
                .tex = glm::vec2 (0.0f + tex_w_off, 1.0f - tex_h_off),
                .verticality = 1.0f,
                .texture = 0
            };
            
            Render::SpriteVertex top_right {
                //.co = glm::vec3(0.0f, 0.0f, 0.0f),
                .co = particle.coords,
                .voffset = glm::vec2 (half_width, half_height),
                .tex = glm::vec2 (tex_width + tex_w_off, 1.0f - tex_h_off),
                .verticality = 1.0f,
                .texture = 0
            };
            
            Render::SpriteVertex bottom_left {
                //.co = glm::vec3(0.0f, 0.0f, 0.0f),
                .co = particle.coords,
                .voffset = glm::vec2 (-half_width, -half_height),
                .tex = glm::vec2 (0.0f + tex_w_off, 1.0f - tex_height - tex_h_off),
                .verticality = 1.0f,
                .texture = 0
            };
            
            Render::SpriteVertex bottom_right {
                //.co = glm::vec3(0.0f, 0.0f, 0.0f),
                .co = particle.coords,
                .voffset = glm::vec2 (half_width, -half_height),
                .tex = glm::vec2 (tex_width + tex_w_off, 1.0f - tex_height - tex_h_off),
                .verticality = 1.0f,
                .texture = 0
            };

            vertices.push_back(top_left);
            vertices.push_back(bottom_left);
            vertices.push_back(top_right);
            vertices.push_back(top_right);
            vertices.push_back(bottom_left);
            vertices.push_back(bottom_right);
        }
        

        glBindBuffer(GL_ARRAY_BUFFER, robject->vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(Render::SpriteVertex)*vertices.size(), &vertices[0], GL_DYNAMIC_DRAW);
        
        robject->eboLen = 6 * vertices.size();
        
        robject->location = location;
        robject->rotation = glm::quat(glm::vec3(0.0f, 0.0f, 0.0f));
    }
}
