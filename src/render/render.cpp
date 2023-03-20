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

using namespace tram;
using namespace tram::Render;

namespace tram::Render {
    
    // cached values for the renderer
    static vec3 CAMERA_POSITION [7] = {{0.0f, 0.0f, 0.0f}};
    static quat CAMERA_ROTATION [7] = {{1.0f, 0.0f, 0.0f, 0.0f}};

    static vec3 SUN_DIRECTION [7] = {{0.0f, 1.0f, 0.0f}};
    static vec3 SUN_COLOR [7] = {{1.0f, 1.0f, 1.0f}};
    static vec3 AMBIENT_COLOR [7] = {{0.0f, 0.0f, 0.0f}};

    float SCREEN_WIDTH = 800.0f;
    float SCREEN_HEIGHT = 600.0f;

    bool THIRD_PERSON = false;
    bool DRAW_RENDER_DEBUG = false;

    Pool<LightListObject> lightPool("lightpool", 100, true);
    Octree<uint32_t> lightTree;


    uint32_t colorlines_vertex_array = 0;
    uint32_t colorlines_vertex_buffer = 0;
    drawlistentry_t colorlines_entry;
    
    std::vector<LineVertex> colorlines;

    /// Initializes the rendering system.
    /// @note Core and UI systems need to be initialized before initializing the render system.
    void Init () {
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

    /// Renders a single frame.
    void Render () {
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
    
    /// Sets the sun direction.
    /// @param direction    Normal vector pointing towards the sun.
    /// @param layer        Rendering layer to which the sun direction will be applied.
    void SetSunDirection (color_t direction, layer_t layer) {
        SUN_DIRECTION [layer] = direction;
        RENDERER.SetLightingParameters(SUN_DIRECTION[layer], SUN_COLOR[layer], AMBIENT_COLOR[layer], layer);
    }
    
    /// Sets the sun color.
    /// @param color    Color of the sun.
    /// @param layer    Rendering layer to which the sun color will be applied.
    void SetSunColor (color_t color, layer_t layer) {
        SUN_COLOR [layer] = color;
        RENDERER.SetLightingParameters(SUN_DIRECTION[layer], SUN_COLOR[layer], AMBIENT_COLOR[layer], layer);
    }
    
    /// Sets the ambient color.
    /// @param color    Ambient color.
    /// @param layer    Rendering layer to which the ambient color will be applied.
    void SetAmbientColor (color_t color, layer_t layer) {
        AMBIENT_COLOR [layer] = color;
        RENDERER.SetLightingParameters(SUN_DIRECTION[layer], SUN_COLOR[layer], AMBIENT_COLOR[layer], layer);
    }

    /// Sets the camera position.
    /// @param position Camera position.
    /// @param layer    Rendering layer to which the camera position will be applied.
    void SetCameraPosition (vec3 position, layer_t layer) {
        CAMERA_POSITION [layer] = position;
        RENDERER.SetCameraParameters(CAMERA_POSITION[layer], CAMERA_ROTATION[layer], layer);
    }
    
    /// Sets the camera rotation.
    /// @param rotation Camera rotation.
    /// @param layer    Rendering layer to which the camera rotation will be applied.
    void SetCameraRotation (quat rotation, layer_t layer) {
        CAMERA_ROTATION [layer] = rotation;
        RENDERER.SetCameraParameters(CAMERA_POSITION[layer], CAMERA_ROTATION[layer], layer);
    }

    /// Returns the camera position for a given layer.
    vec3 GetCameraPosition (layer_t layer) {
        return CAMERA_POSITION [layer];
    }
    
    /// Returns the camera rotation for a given layer.
    quat GetCameraRotation (layer_t layer) {
        return CAMERA_ROTATION [layer];
    }
    
}



