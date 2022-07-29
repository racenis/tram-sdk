// TRAMWAY DRIFT AND DUNGEON EXPLORATION SIMULATOR 2022
// All rights reserved.
//
// RENDER.CPP -- Rendering stuff.

#include <core.h>
#include <render.h>

#include <glm/gtc/type_ptr.hpp>

// too too
#include <glad.c>

using namespace Core;

namespace Core::Render {
    glm::vec3 CAMERA_POSITION = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::quat CAMERA_ROTATION = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);

    float CAMERA_PITCH = 0.0f;
    float CAMERA_YAW = -90.0f;

    float SCREEN_WIDTH = 800.0f;  // move to UI?
    float SCREEN_HEIGHT = 600.0f;

    bool THIRD_PERSON = false;

    Material FONT_REGULAR;
    Material FONT_TITLE;
    Material FONT_SYMBOLS;

    bool DRAW_PHYSICS_DEBUG = false;
    bool DRAW_PATH_DEBUG = false;

    float FRAME_LIMIT = 60.0f;

    float time_of_day = 0.8f;

    ShaderUniformMatrices matrices;
    ShaderUniformModelMatrices modelMatrices;

    Pool<PoseListObject> poseList("pose list", 100, true);
    Pool<RenderListObject> renderList("render list", 500, false);
    Pool<LightListObject> lightPool("lightpool", 100, true);
    Octree<uint32_t> lightTree;

    std::vector<LineVertex> colorlines;
    std::vector<TextVertex> textvertices;
    
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
            {1, GL_UNSIGNED_INT, sizeof(TextVertex), (void*)offsetof(StaticModelVertex, texture)}
        }, std::vector<Material*>{
            &FONT_REGULAR,
            &FONT_TITLE,
            &FONT_TITLE, // <--- just a placeholder
            &FONT_TITLE, // <--- also a placeholder
            &FONT_SYMBOLS
        }, GL_TRIANGLES, textvertices.size(), textvertices.size() * sizeof(TextVertex), &textvertices[0]);

    void Init(){

        CompileShaders();

        //glUseProgram(SHADER_NORMAL_STATIC);

        lineBuffer.Init();
        textBuffer.Init();
        
        lightUniform.Init();
        matricesUniform.Init();
        modelMatricesUniform.Init();
        boneUniform.Init();

        matrices.projection = glm::perspective(glm::radians(45.0f), SCREEN_WIDTH / SCREEN_HEIGHT, 0.1f, 1000.0f);

        poseList.AddNew();
        for (size_t i = 0; i < 30; i++){
            poseList.begin()->pose[i] = glm::mat4(1.0f);
        }
        
        

        FONT_REGULAR = Material(UID("jost"), Material::TEXTURE_MSDF);  // futura knock-off
        FONT_TITLE = Material(UID("inter"), Material::TEXTURE_MSDF);    // helvetica
        FONT_SYMBOLS = Material(UID("symbols"), Material::TEXTURE_MSDF);  // ornamentation and other non-text glyphs

        FONT_REGULAR.Load();
        FONT_TITLE.Load();
        FONT_SYMBOLS.Load();

    }

    void Render(){
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

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


        // draw paths
        if(DRAW_PATH_DEBUG){
            PathNode* pp = pathNodePool.begin();
            while (pp < pathNodePool.end()){
                char buffer[100];
                sprintf(buffer, "ID: %d", pp->id);
                AddLine(pp->coords, pp->coords + glm::vec3(0.0f, 1.0f, 0.0f), pp->type == 'l' ? COLOR_BLUE : pp->type == 'r' ? COLOR_RED : COLOR_PINK);
                UI::SetDebugText(buffer, pp->coords, COLOR_GREEN);
                for (size_t i = 0; i < 3; i++){
                    if(pp->next[i] != nullptr){
                        glm::vec3 f = pp->coords;
                        glm::vec3 u = pp->next[i]->coords;
                        AddLine(f, u, COLOR_YELLOW);
                    }
                }
                pp++;
            }
        }


        // set opengl to draw everything on top and as a line
        glDisable(GL_DEPTH_TEST);
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

        glUseProgram(ShaderProgram::Find(Model::LINE_VERTEX, Material::FLAT_COLOR)->compiled_shader);
        

        // upload the line buffer and draw lines
        lineBuffer.UpdateData(colorlines.size(), colorlines.size() * sizeof(LineVertex), &colorlines[0]);
        lineBuffer.Upload();
        lineBuffer.Draw();

        // back to drawing triangles
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        // set special text shader
        glUseProgram(ShaderProgram::Find(Model::TEXT_VERTEX, Material::TEXTURE_MSDF)->compiled_shader);

        // upload the text array and draw the text
        textBuffer.UpdateData(textvertices.size(), textvertices.size() * sizeof(TextVertex), &textvertices[0]);
        textBuffer.Upload();
        textBuffer.Draw();

        // back to drawing everything in its supposed place
        glEnable(GL_DEPTH_TEST);

        Stats::Stop(Stats::RENDER);

        // clear the buffers for the next frame
        colorlines.clear();
        textvertices.clear();

        Stats::Stop(Stats::FRAME_NO_SWAP);

        #ifdef FRAMELIMIT
        float skip = ((1.0f / FRAME_LIMIT) - (float)Stats::total_time[STAT_FRAME_NO_SWAP]) * 1000.0f;
        //std::cout << skip << " " << (float)Stats::total_time[STAT_FRAME_NO_SWAP] << std::endl;
        if (skip > 16.0f) skip = 16.0f;
        if (skip > 1.0f) std::this_thread::sleep_for(std::chrono::milliseconds((int)skip));
        #endif


    }

}


namespace Core {


}
