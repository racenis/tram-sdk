#include <render/renderer.h>

// too too
#include <glad.c>
//#include <glad_gles3.c>

#include <glm/gtc/type_ptr.hpp>

#include <components/spritecomponent.h>
#include <components/particlecomponent.h>

#include <gui.h>

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
    
    
    void ScreenSize(float width, float height) {
        matrices.projection = glm::perspective(glm::radians(45.0f), SCREEN_WIDTH / SCREEN_HEIGHT, 0.1f, 1000.0f);
    }
    

        
    void Init(){
        
        CompileShaders();

        light_uniform_buffer = MakeUniformBuffer("Lights", light_uniform_binding, sizeof(LightListObject)*50);
        matrix_uniform_buffer = MakeUniformBuffer("Matrices", matrix_uniform_binding, sizeof(ShaderUniformMatrices));
        model_matrix_uniform_buffer = MakeUniformBuffer("ModelMatrices", model_matrix_uniform_binding, sizeof(ShaderUniformModelMatrices));
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

        if (THIRD_PERSON) matrices.view = glm::translate(matrices.view, CAMERA_ROTATION * glm::vec3(0.0f, 0.0f, -5.0f));

        UploadUniformBuffer(matrix_uniform_buffer, sizeof(ShaderUniformMatrices), &matrices);
        UploadUniformBuffer(light_uniform_buffer, sizeof(LightListObject)*50, &lightPool.begin()->location);

        static uint32_t layer; layer = 0;

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
            
            #ifndef ENGINE_EDITOR_MODE
            if (DRAW_RENDER_DEBUG) {
                char debug_text_buffer[200];
                sprintf(debug_text_buffer, "Robj vao:%d location-xyz: %f %f %f lights: %d %d %d %d", robj->vao, robj->location[0], robj->location[1], robj->location[2], robj->lights[0], robj->lights[1], robj->lights[2], robj->lights[3]);
                GUI::DebugText(debug_text_buffer, robj->location, COLOR_GREEN);
            }
            #endif // ENGINE_EDITOR_MODE

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

            if (robj->flags & FLAG_INTERIOR_LIGHTING) {
                modelMatrices.sunWeight = 0.0f;
            } else {
                modelMatrices.sunWeight = 1.0f;
            }

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
            
            
            if (layer != robj->layer) {
                // *whatever opengl call clears the depth buffer*
                layer = robj->layer;
            }

            if (robj->flags & FLAG_NO_DEPTH_TEST) glDisable(GL_DEPTH_TEST);
            if (robj->flags & FLAG_DRAW_INDEXED) {
                glBindVertexArray(robj->vao);
                glDrawElements(robj->flags & FLAG_DRAW_LINES ? GL_LINES : GL_TRIANGLES, robj->eboLen * 3, GL_UNSIGNED_INT, (void*)(robj->eboOff * 3 * sizeof(uint32_t)));
            } else {
                glBindVertexArray(robj->vao);
                glDrawArrays(robj->flags & FLAG_DRAW_LINES ? GL_LINES : GL_TRIANGLES, 0, robj->eboLen);
            }
            if (robj->flags & FLAG_NO_DEPTH_TEST) glEnable(GL_DEPTH_TEST);


        }

        Stats::Stop(Stats::RENDER);
        Stats::Stop(Stats::FRAME_NO_SWAP);
    }
    
    DrawListEntryHandle InsertDrawListEntry() {
        return DrawListEntryHandle { .draw_list_entries = { renderList.AddNew(), nullptr, nullptr, nullptr, nullptr, nullptr}};
    }
    
    DrawListEntryHandle InsertDrawListEntry(Model* model) {
        assert(model);
        assert(model->GetStatus() == Resource::READY);
        
        DrawListEntryHandle entries;
        
        for (size_t i = 0; i < model->element_ranges.size(); i++) {
            RenderListObject* entry = Render::renderList.AddNew();
            
            entry->vao = model->vertex_array_handle;
            entry->shader = FindShader(model->vertex_format, model->element_ranges[i].material_type);
            entry->texCount = model->element_ranges[i].material_count;
            entry->eboLen = model->element_ranges[i].element_length;
            entry->eboOff = model->element_ranges[i].element_offset;
            entry->flags = FLAG_RENDER | FLAG_DRAW_INDEXED;
            
            for (size_t k = 0; k < model->element_ranges[i].material_count; k++) {
                entry->textures[k] = model->materials[model->element_ranges[i].materials[k]]->GetTexture();
            }
            
            entries.draw_list_entries[i] = entry;
        }
        
        return entries;
    }
    
    void RemoveDrawListEntry(DrawListEntryHandle entry) {
        for (size_t i = 0; i < 6 && entry.draw_list_entries[i]; i++) {
            renderList.Remove((RenderListObject*)entry.draw_list_entries[i]);
        }
    }
    
    uint32_t GetFlags(DrawListEntryHandle entry) {
        assert(entry.draw_list_entries[0]);
        return ((RenderListObject*)entry.draw_list_entries[0])->flags;
    }
    
    void SetFlags(DrawListEntryHandle entry, uint32_t flags) {
        for (size_t i = 0; i < 6 && entry.draw_list_entries[i]; i++) {
            ((RenderListObject*)entry.draw_list_entries[i])->flags = flags;
        }
    }
    
    void SetPose(DrawListEntryHandle entry, PoseListObject* pose) {
        for (size_t i = 0; i < 6 && entry.draw_list_entries[i]; i++) {
            ((RenderListObject*)entry.draw_list_entries[i])->pose = pose;
        }
    }
    
    void SetLightmap(DrawListEntryHandle entry, uint32_t lightmap) {
        for (size_t i = 0; i < 6 && entry.draw_list_entries[i]; i++) {
            ((RenderListObject*)entry.draw_list_entries[i])->lightmap = lightmap;
        }
    }
    
    void SetLights(DrawListEntryHandle entry, uint32_t* lights) {
        for (size_t i = 0; i < 6 && entry.draw_list_entries[i]; i++) {
            for (size_t k = 0; k < 4; k++) {
                ((RenderListObject*)entry.draw_list_entries[i])->lights[k] = lights[k];
            }
        }
    }
    
    void SetLocation(DrawListEntryHandle entry, glm::vec3& location) {
        for (size_t i = 0; i < 6 && entry.draw_list_entries[i]; i++) {
            ((RenderListObject*)entry.draw_list_entries[i])->location = location;
        }
    }
    
    void SetRotation(DrawListEntryHandle entry, glm::quat& rotation) {
        for (size_t i = 0; i < 6 && entry.draw_list_entries[i]; i++) {
            ((RenderListObject*)entry.draw_list_entries[i])->rotation = rotation;
        }
    }
    
    void SetDrawListVertexArray(DrawListEntryHandle entry, uint32_t vertex_array_handle) {
        ((RenderListObject*)entry.draw_list_entries[0])->vao = vertex_array_handle;
    }
    
    void SetDrawListElements(DrawListEntryHandle entry, uint32_t element_offset, uint32_t element_length) {
        ((RenderListObject*)entry.draw_list_entries[0])->eboOff = element_offset;
        ((RenderListObject*)entry.draw_list_entries[0])->eboLen = element_length;
    }
    
    void SetDrawListShader(DrawListEntryHandle entry, Model::VertexFormat vertex_format, Material::Type material_type) {
        ((RenderListObject*)entry.draw_list_entries[0])->shader = FindShader(vertex_format, material_type);
    }
    
    void SetDrawListTextures(DrawListEntryHandle entry, size_t texture_count, uint32_t* textures) {
        for (size_t i = 0; i < texture_count; i++) {
            ((RenderListObject*)entry.draw_list_entries[0])->textures[i] = textures[i];
        }
        ((RenderListObject*)entry.draw_list_entries[0])->texCount = texture_count;
    }
    
    uint32_t CreateTexture(ColorMode color_mode, TextureFilter texture_filter, uint32_t width, uint32_t height, void* data) {
        uint32_t texture;
        uint32_t opengl_tex_format;
        
        switch (color_mode) {
            case COLORMODE_R:
                opengl_tex_format = GL_RED;
                break;
            case COLORMODE_RG:
                opengl_tex_format = GL_RG;
                break;
            case COLORMODE_RGB:
                opengl_tex_format = GL_RGB;
                break;
            case COLORMODE_RGBA:
                opengl_tex_format = GL_RGBA;
        }
        
        // TODO: add a similar switch() to the texture_filter, so that you can change them
        
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        assert(data);

        glTexImage2D(GL_TEXTURE_2D, 0, opengl_tex_format, width, height, 0, opengl_tex_format, GL_UNSIGNED_BYTE, data);
            
        glGenerateMipmap(GL_TEXTURE_2D);
        
        return texture;
    }
    
    void CreateIndexedVertexArray(const VertexDefinition& vertex_format, uint32_t& vertex_buffer_handle, uint32_t& element_buffer_handle,  uint32_t& vertex_array_handle, size_t vertex_size, void* vertex_data, size_t index_size, void* index_data) {
        glGenBuffers(1, &vertex_buffer_handle);
        glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_handle);
        glBufferData(GL_ARRAY_BUFFER, vertex_size, vertex_data, GL_STATIC_DRAW);

        glGenBuffers(1, &element_buffer_handle);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, element_buffer_handle);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, index_size, index_data, GL_STATIC_DRAW);

        glGenVertexArrays(1, &vertex_array_handle);

        glBindVertexArray(vertex_array_handle);

        for (size_t i = 0; i < vertex_format.size(); i++) {
            uint32_t opengl_type = vertex_format[i].type == VertexProperty::FLOAT32 ? GL_FLOAT : GL_UNSIGNED_INT;
            
            if (opengl_type == GL_FLOAT) {
                glVertexAttribPointer(i, vertex_format[i].size, opengl_type, GL_FALSE, vertex_format[i].stride, (void*)vertex_format[i].offset);
            } else {
                glVertexAttribIPointer(i, vertex_format[i].size, opengl_type, vertex_format[i].stride, (void*)vertex_format[i].offset);
            }
            
            glEnableVertexAttribArray(i);
        }

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, element_buffer_handle);
        
        glBindVertexArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    }
    
    void CreateVertexArray(const VertexDefinition& vertex_format, uint32_t& vertex_buffer_handle,  uint32_t& vertex_array_handle) {
        glGenBuffers(1, &vertex_buffer_handle);
        glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_handle);
        glBufferData(GL_ARRAY_BUFFER, 0, nullptr, GL_DYNAMIC_DRAW);

        glGenVertexArrays(1, &vertex_array_handle);

        glBindVertexArray(vertex_array_handle);

        for (size_t i = 0; i < vertex_format.size(); i++) {
            uint32_t opengl_type = vertex_format[i].type == VertexProperty::FLOAT32 ? GL_FLOAT : GL_UNSIGNED_INT;
            
            if (opengl_type == GL_FLOAT) {
                glVertexAttribPointer(i, vertex_format[i].size, opengl_type, GL_FALSE, vertex_format[i].stride, (void*)vertex_format[i].offset);
            } else {
                glVertexAttribIPointer(i, vertex_format[i].size, opengl_type, vertex_format[i].stride, (void*)vertex_format[i].offset);
            }
            
            glEnableVertexAttribArray(i);
        }

        glBindVertexArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }
    
    void UpdateVertexArray(uint32_t vertex_buffer_handle, size_t data_size, void* data) {
        glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_handle);
        glBufferData(GL_ARRAY_BUFFER, data_size, data, GL_DYNAMIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }
}

    void Core::Render::Project(const glm::vec3& point, glm::vec3& result) {
        result = glm::project(point, OpenGL::matrices.view, OpenGL::matrices.projection, glm::vec4 (0.0f, 0.0f, Render::SCREEN_WIDTH, Render::SCREEN_HEIGHT));
        result.y = Render::SCREEN_HEIGHT - result.y;
    }

namespace Core {
    using namespace Core::Render::OpenGL;
    using namespace Core::Render;
    
    void SpriteComponent::Uninit(){
        is_ready = false;
        //Render::renderList.Remove(robject);
        OpenGL::RemoveDrawListEntry(draw_list_entry);
    };

    void SpriteComponent::Start(){
        assert(!is_ready);

        //robject = Render::renderList.AddNew();
        
        CreateVertexArray(SPRITE_VERTEX_DEFINITION, vertex_buffer, vertex_array);
        
        auto texture_handle = sprite->GetMaterial()->GetTexture();
        
        draw_list_entry = OpenGL::InsertDrawListEntry();
        OpenGL::SetDrawListVertexArray(draw_list_entry, vertex_array);
        OpenGL::SetDrawListElements(draw_list_entry, 0, 6);
        OpenGL::SetFlags(draw_list_entry, FLAG_RENDER);
        OpenGL::SetDrawListTextures(draw_list_entry, 1, &texture_handle);
        OpenGL::SetDrawListShader(draw_list_entry, Model::SPRITE_VERTEX, Material::TEXTURE_ALPHA);
        //robject->vao = vertex_array;
        //robject->flags = Render::FLAG_RENDER;
        //robject->eboLen = 6;
        //robject->eboOff = 0;
        //robject->texCount = 1;
        //robject->textures[0] = sprite->GetMaterial()->GetTexture();
        //robject->shader = FindShader(Render::Model::SPRITE_VERTEX, Render::Material::TEXTURE_ALPHA);
        
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
                if (!(anim_frame < sprite->frames.size())) anim_frame = 0;
            }
            anim_bframe++;
        }
    }

    void SpriteComponent::UpdateRenderListObject(){
        if (!is_ready) return;

        // maybe cache these values, instead of re-calculating them for each frame?
        float tex_width = (float)sprite->frames[anim_frame].width / (float)sprite->GetMaterial()->GetWidth();//sprite->width;
        float tex_height = (float)sprite->frames[anim_frame].height / (float)sprite->GetMaterial()->GetHeight(); //sprite->height;
        float tex_w_off = (float)sprite->frames[anim_frame].offset_x / (float)sprite->GetMaterial()->GetWidth();//(float)(anim_frame % sprite->frames_w) * tex_width;
        float tex_h_off = (float)sprite->frames[anim_frame].offset_y / (float)sprite->GetMaterial()->GetHeight();//(float)(anim_frame / sprite->frames_w) * tex_height;
        float half_width = tex_width * sprite->frames[anim_frame].scale / 2.0f;
        float half_height = tex_height * sprite->frames[anim_frame].scale / 2.0f;

        Render::SpriteVertex top_left {
            .co = glm::vec3(0.0f, 0.0f, 0.0f),
            .voffset = glm::vec2 (-half_width, half_height),
            .texco = glm::vec2 (0.0f + tex_w_off, 1.0f - tex_h_off),
            .verticality = 1.0f,
            .texture = 0
        };
        
        Render::SpriteVertex top_right {
            .co = glm::vec3(0.0f, 0.0f, 0.0f),
            .voffset = glm::vec2 (half_width, half_height),
            .texco = glm::vec2 (tex_width + tex_w_off, 1.0f - tex_h_off),
            .verticality = 1.0f,
            .texture = 0
        };
        
        Render::SpriteVertex bottom_left {
            .co = glm::vec3(0.0f, 0.0f, 0.0f),
            .voffset = glm::vec2 (-half_width, -half_height),
            .texco = glm::vec2 (0.0f + tex_w_off, 1.0f - tex_height - tex_h_off),
            .verticality = 1.0f,
            .texture = 0
        };
        
        Render::SpriteVertex bottom_right {
            .co = glm::vec3(0.0f, 0.0f, 0.0f),
            .voffset = glm::vec2 (half_width, -half_height),
            .texco = glm::vec2 (tex_width + tex_w_off, 1.0f - tex_height - tex_h_off),
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

        //glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
        //glBufferData(GL_ARRAY_BUFFER, sizeof(Render::SpriteVertex)*vertices.size(), &vertices[0], GL_DYNAMIC_DRAW);
        OpenGL::UpdateVertexArray(vertex_buffer, sizeof(SpriteVertex) * vertices.size(), &vertices[0]);
        
        OpenGL::SetLocation(draw_list_entry, location);
        //robject->location = location;
        //robject->rotation = glm::quat(glm::vec3(0.0f, 0.0f, 0.0f));
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

        //Render::renderList.Remove(robject);
        OpenGL::RemoveDrawListEntry(draw_list_entry);
    };

    void ParticleComponent::Start(){
        if(is_ready) return;


        //robject = Render::renderList.AddNew();


        CreateVertexArray(SPRITE_VERTEX_DEFINITION, vertex_buffer, vertex_array);
        //auto vert_array = MakeVertexArray(vertex_formats[Model::SPRITE_VERTEX], 6, true);
        
        //vertex_array = vert_array.array_handle;
        //vertex_buffer = vert_array.buffer_handle;
        
        //robject->vao = vertex_array;
        
        auto texture_handle = sprite->GetMaterial()->GetTexture();
        
        draw_list_entry = OpenGL::InsertDrawListEntry();
        OpenGL::SetDrawListVertexArray(draw_list_entry, vertex_array);
        OpenGL::SetDrawListElements(draw_list_entry, 0, 6);
        OpenGL::SetFlags(draw_list_entry, FLAG_RENDER);
        OpenGL::SetDrawListTextures(draw_list_entry, 1, &texture_handle);
        OpenGL::SetDrawListShader(draw_list_entry, Model::SPRITE_VERTEX, Material::TEXTURE_ALPHA);
        
        //robject->flags = Render::FLAG_RENDER;
        //robject->lightmap = 0;
        //robject->ebo = 0;
        //robject->eboLen = 6;
        //robject->eboOff = 0;
        //robject->texCount = 1;
        //robject->textures[0] = sprite->GetMaterial()->GetTexture();
        //robject->lights[0] = 0;
        //robject->lights[1] = 0;
        //robject->lights[2] = 0;
        //robject->lights[3] = 0;
       // robject->shader = FindShader(Render::Model::SPRITE_VERTEX, Render::Material::TEXTURE_ALPHA);
        
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
        
        float tex_width = sprite->frames.front().width / (float)sprite->GetMaterial()->GetWidth();
        float tex_height = sprite->frames.front().height / (float)sprite->GetMaterial()->GetHeight();
        float tex_w_off = sprite->frames.front().offset_x / (float)sprite->GetMaterial()->GetWidth();
        float tex_h_off = sprite->frames.front().offset_y / (float)sprite->GetMaterial()->GetHeight();
        float half_width = tex_width * sprite->frames.front().scale / 2.0f;
        float half_height = tex_height * sprite->frames.front().scale / 2.0f;

        std::vector<Render::SpriteVertex> vertices;

        for (auto particle : particles) {
            Render::SpriteVertex top_left {
                //.co = glm::vec3(0.0f, 0.0f, 0.0f),
                .co = particle.coords,
                .voffset = glm::vec2 (-half_width, half_height),
                .texco = glm::vec2 (0.0f + tex_w_off, 1.0f - tex_h_off),
                .verticality = 1.0f,
                .texture = 0
            };
            
            Render::SpriteVertex top_right {
                //.co = glm::vec3(0.0f, 0.0f, 0.0f),
                .co = particle.coords,
                .voffset = glm::vec2 (half_width, half_height),
                .texco = glm::vec2 (tex_width + tex_w_off, 1.0f - tex_h_off),
                .verticality = 1.0f,
                .texture = 0
            };
            
            Render::SpriteVertex bottom_left {
                //.co = glm::vec3(0.0f, 0.0f, 0.0f),
                .co = particle.coords,
                .voffset = glm::vec2 (-half_width, -half_height),
                .texco = glm::vec2 (0.0f + tex_w_off, 1.0f - tex_height - tex_h_off),
                .verticality = 1.0f,
                .texture = 0
            };
            
            Render::SpriteVertex bottom_right {
                //.co = glm::vec3(0.0f, 0.0f, 0.0f),
                .co = particle.coords,
                .voffset = glm::vec2 (half_width, -half_height),
                .texco = glm::vec2 (tex_width + tex_w_off, 1.0f - tex_height - tex_h_off),
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
        

        //glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
        //glBufferData(GL_ARRAY_BUFFER, sizeof(Render::SpriteVertex)*vertices.size(), &vertices[0], GL_DYNAMIC_DRAW);
        
        //robject->eboLen = 6 * vertices.size();
        
        OpenGL::UpdateVertexArray(vertex_buffer, sizeof(SpriteVertex) * vertices.size(), &vertices[0]);
        OpenGL::SetDrawListElements(draw_list_entry, 0, /*6 * */vertices.size());
        
        
        OpenGL::SetLocation(draw_list_entry, location);
        
        //robject->location = location;
        //robject->rotation = glm::quat(glm::vec3(0.0f, 0.0f, 0.0f));
    }
}
