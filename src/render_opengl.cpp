#include <render.h>
#include <components/rendercomponent.h>

// too too
#include <glad.c>
//#include <glad_gles3.c>

#include <glm/gtc/type_ptr.hpp>

namespace Core::Render {
    Pool<RenderListObject> renderList("render list", 500, false);
    Pool<LightListObject> lightPool("lightpool", 100, true);
    Octree<uint32_t> lightTree;

    std::vector<LineVertex> colorlines;
    std::vector<TextVertex> textvertices;
    std::vector<GlyphVertex> glyphvertices;
    
    // TODO: move into opengl-specific code
    struct ShaderUniformMatrices {
        glm::mat4 projection;       /// Projection matrix.
        glm::mat4 view;             /// View matrix.
    };

    // TODO: move into opengl-specific code
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
    
    class UniformBuffer : ShaderBuffer {
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
            ShaderProgram::BindUniformBlock([](ShaderProgram& shader){ return true; }, name.c_str(), binding);

            glBindBuffer(GL_UNIFORM_BUFFER, 0);
        }
        
        void Upload () {
            glBindBuffer(GL_UNIFORM_BUFFER, handle);
            glBufferSubData(GL_UNIFORM_BUFFER, 0, data_size, data);
            glBindBuffer(GL_UNIFORM_BUFFER, 0);
        }
    };
    
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
    
    // should be bound to dynamic
    auto lightUniform = UniformBuffer(2, "Lights", sizeof(LightListObject)*50, lightPool.begin());
    
    // should be bound to static, static_alpha, dynamic, water
    auto matricesUniform = UniformBuffer(0, "Matrices", sizeof(ShaderUniformMatrices), &matrices);
    
    // should be bound to static, static_alpha, dynamic, water
    auto modelMatricesUniform = UniformBuffer(1, "ModelMatrices", sizeof(ShaderUniformModelMatrices), &modelMatrices);
    
    // should be bound to dynamic
    auto boneUniform = UniformBuffer(3, "Bones", sizeof(PoseListObject), nullptr);

    auto lineBuffer = VertexBuffer(std::vector<VertexBuffer::VertexFormat>{
            {3, GL_FLOAT, sizeof(LineVertex), nullptr},
            {3, GL_FLOAT, sizeof(LineVertex), (void*)offsetof(LineVertex, color)}
            }, std::vector<Material*>(), GL_LINES, colorlines.size(), colorlines.size() * sizeof(LineVertex), &colorlines[0]);
            
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
        SCREEN_WIDTH = width;
        SCREEN_HEIGHT = height;
        matrices.projection = glm::perspective(glm::radians(45.0f), SCREEN_WIDTH / SCREEN_HEIGHT, 0.1f, 1000.0f);
    }
        
    void RenderListObject::FillFromModel(Model* mdl, uint32_t eboIndex){
        vbo = mdl->vbo;
        ebo = mdl->ebo;
        vao = mdl->vao;
        eboLen = mdl->eboLen[eboIndex];
        eboOff = mdl->eboOff[eboIndex];
        shader = FindShader(mdl->vertForm, (Material::Type)mdl->eboMat[eboIndex]);
        texCount = mdl->materials.size();
        for (uint32_t i = 0; i < texCount; i++){
            textures[i] = mdl->materials[i]->GetTexture();
        }

        flags = FLAG_RENDER; // TODO: get the RFLAG_INTERIOR_LIGHTING in here somehow
    };
        
    void Init_OpenGL(){
            
        CompileShaders();

        //glUseProgram(SHADER_NORMAL_STATIC);

        lineBuffer.Init();
        textBuffer.Init();
        glyphBuffer.Init();
        
        lightUniform.Init();
        matricesUniform.Init();
        modelMatricesUniform.Init();
        boneUniform.Init();

        matrices.projection = glm::perspective(glm::radians(45.0f), SCREEN_WIDTH / SCREEN_HEIGHT, 0.1f, 1000.0f);

        poseList.AddNew();
        for (size_t i = 0; i < 30; i++){
            poseList.begin()->pose[i] = glm::mat4(1.0f);
        }
        
    }
    
    void Render_OpenGL() {
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

        matricesUniform.Upload();

        lightUniform.Upload();

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
            //UI::SetDebugText(debug_text_buffer, robj->location, COLOR_GREEN);

            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, glm::vec3(robj->location[0], robj->location[1], robj->location[2]));

            model *= glm::toMat4(robj->rotation);

            glUseProgram(robj->shader);

            if(robj->pose != nullptr){
                boneUniform.data = glm::value_ptr(robj->pose->pose[0]);
                boneUniform.Upload();
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
            modelMatricesUniform.Upload();



            for (unsigned int j = 0; j < robj->texCount; j++){
                glActiveTexture(GL_TEXTURE0 + j);
                glBindTexture(GL_TEXTURE_2D, robj->textures[j]);
            }

            if(robj->lightmap != 0){
                glActiveTexture(GL_TEXTURE15);
                glBindTexture(GL_TEXTURE_2D, robj->lightmap);
            }


            glBindVertexArray(robj->vao);
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, robj->ebo);
            glDrawElements(GL_TRIANGLES, robj->eboLen * 3, GL_UNSIGNED_INT, (void*)(robj->eboOff * 3 * sizeof(uint32_t)));



        }

        // set opengl to draw everything on top and as a line
        glDisable(GL_DEPTH_TEST);
        //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

        glUseProgram(ShaderProgram::Find(Model::LINE_VERTEX, Material::FLAT_COLOR)->compiled_shader);
        

        // upload the line buffer and draw lines
        lineBuffer.UpdateData(colorlines.size(), colorlines.size() * sizeof(LineVertex), &colorlines[0]);
        lineBuffer.Upload();
        lineBuffer.Draw();

        // back to drawing triangles
        //glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        // set special text shader
        glUseProgram(ShaderProgram::Find(Model::TEXT_VERTEX, Material::TEXTURE_MSDF)->compiled_shader);

        // upload the text array and draw the text
        textBuffer.UpdateData(textvertices.size(), textvertices.size() * sizeof(TextVertex), &textvertices[0]);
        textBuffer.Upload();
        textBuffer.Draw();
        
        glUseProgram(ShaderProgram::Find(Model::GLYPH_VERTEX, Material::TEXTURE_GLYPH)->compiled_shader);
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

namespace Core {
    void RenderComponent::Uninit(){
            is_ready = true;
            pose = nullptr;

            for (size_t i = 0; i < 7; i++){
                if(rLsObjPtr[i] == nullptr) continue;

                Render::renderList.Remove(rLsObjPtr[i]);

                rLsObjPtr[i] = nullptr;
            }
        };

        void RenderComponent::Start(){
            if(is_ready) return;


            uint32_t pointers = 0;
            for(uint32_t i = 0; i < 6; i++){
                if (model->IsEBOEmpty(i)) continue;
                rLsObjPtr[pointers] = Render::renderList.AddNew();
                rLsObjPtr[pointers]->FillFromModel(model.GetResource(), i);
                if(lightmap.GetResource() != nullptr) rLsObjPtr[pointers]->lightmap = lightmap->GetTexture();
                rLsObjPtr[pointers]->pose = pose;
                pointers++;

            }

            is_ready = true;

            UpdateRenderListObjs();

            return;
        }

        void RenderComponent::UpdateRenderListObjs(){
            if (!is_ready) return;

            for(uint32_t i = 0; rLsObjPtr[i] != nullptr; i++){
                rLsObjPtr[i]->location = location;
                rLsObjPtr[i]->rotation = rotation;

                rLsObjPtr[i]->pose = pose;

                rLsObjPtr[i]->flags &= -1 ^ Render::FLAG_INTERIOR_LIGHTING;
                rLsObjPtr[i]->flags |= Render::FLAG_INTERIOR_LIGHTING * isInterior;

                rLsObjPtr[i]->lights[0] = 0;
                rLsObjPtr[i]->lights[1] = 0;
                rLsObjPtr[i]->lights[2] = 0;
                rLsObjPtr[i]->lights[3] = 0;
                Render::lightTree.FindNearest(rLsObjPtr[i]->lights, rLsObjPtr[i]->location.x, rLsObjPtr[i]->location.y, rLsObjPtr[i]->location.z);
                //CalcLights(rLsObjPtr[i]);
            }

        }
    
}
