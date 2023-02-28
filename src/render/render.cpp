// TRAMWAY DRIFT AND DUNGEON EXPLORATION SIMULATOR 2022
// All rights reserved.

#include <framework/core.h>
#include <framework/system.h>
#include <render/render.h>
#include <render/renderer.h>
#include <render/vertices.h>
#include <components/rendercomponent.h>
#include <components/spritecomponent.h>
#include <components/particlecomponent.h>

#include <render/opengl/renderer.h>

using namespace Core;

namespace Core::Render {
    glm::vec3 CAMERA_POSITION = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::quat CAMERA_ROTATION = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);

    float SCREEN_WIDTH = 800.0f;
    float SCREEN_HEIGHT = 600.0f;

    bool THIRD_PERSON = false;
    bool DRAW_RENDER_DEBUG = false;
    
    glm::vec3 SUN_DIRECTION = glm::vec3(0.0f, 1.0f, 0.0f);
    glm::vec3 SUN_COLOR = glm::vec3(1.0f, 1.0f, 1.0f);
    glm::vec3 AMBIENT_COLOR = glm::vec3(0.0f, 0.0f, 0.0f);

    float FRAME_LIMIT = 60.0f;

    Pool<LightListObject> lightPool("lightpool", 100, true);
    Octree<uint32_t> lightTree;


    uint32_t colorlines_vertex_array = 0;
    uint32_t colorlines_vertex_buffer = 0;
    DrawListEntryHandle colorlines_entry;
    
    std::vector<LineVertex> colorlines;

    
    void Init(){
        assert(System::IsInitialized(System::SYSTEM_CORE));
        assert(System::IsInitialized(System::SYSTEM_UI));
        
        // since we only have the OpenGL renderer, we'll init that one
        OpenGL::Init();
        
        
        // this is for rendering lines
        CreateVertexArray (VERTEX_DEFINITION<LineVertex>, colorlines_vertex_buffer, colorlines_vertex_array);
        colorlines_entry = InsertDrawListEntry ();
        SetDrawListVertexArray(colorlines_entry, colorlines_vertex_array);
        SetDrawListShader(colorlines_entry, Model::LINE_VERTEX, Material::FLAT_COLOR);
        SetFlags(colorlines_entry, FLAG_RENDER | FLAG_NO_DEPTH_TEST | FLAG_DRAW_LINES);
        
        Material::LoadErrorMaterial();
        Model::LoadErrorModel();
        
        System::SetInitialized(System::SYSTEM_RENDER, true);
    }

    void Render(){
        #ifndef ENGINE_EDITOR_MODE
        // idk if these need to be here -> after all, the armatures aren't updated in here?
        for (auto& it : PoolProxy<SpriteComponent>::GetPool()) it.Update();
        for (auto& it : PoolProxy<ParticleComponent>::GetPool()) it.Update();
        #endif // ENGINE_EDITOR_MODE
        
        UpdateVertexArray(colorlines_vertex_buffer, colorlines.size() * sizeof(LineVertex), &colorlines[0]);
        SetDrawListElements(colorlines_entry, 0, colorlines.size());
        colorlines.clear();
        
        RenderFrame();
    }
    
    void ScreenSize(float width, float height) {
        SCREEN_WIDTH = width;
        SCREEN_HEIGHT = height;
        SetScreenSize(width, height);
    }
}



