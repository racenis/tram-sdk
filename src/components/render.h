// Tramway Drifting and Dungeon Exploration Simulator SDK Runtime

#ifndef TRAM_SDK_COMPONENTS_RENDERCOMPONENT_H
#define TRAM_SDK_COMPONENTS_RENDERCOMPONENT_H

#include <render/model.h>
#include <render/lightmap.h>
#include <render/environment.h>

namespace tram {

class AnimationComponent;

class RenderComponent : public EntityComponent {
public:
    static RenderComponent* Make();
    static void Yeet(RenderComponent* component);
    
    void SetModel(name_t name);
    void SetModel(Render::Model* model) { this->model = model; }
    void SetLightmap(name_t name);
    void SetLightmap(Render::Lightmap* lightmap);
    void SetEnvironmentMap(Render::Environment* environment);
    void SetArmature(AnimationComponent* armature);
    void SetPose(AnimationComponent* armature);

    inline Render::Model* GetModel() { return model.get(); }
    inline Render::Lightmap* GetLightmap() { return lightmap.get(); }

    void SetLocation(vec3 location);
    void SetRotation(quat rotation);
    void SetScale(vec3 scale);
    void SetColor(vec3 color);
    void SetOpacity(float opacity);
    void SetLayer(uint32_t layer);
    void SetLayers(uint32_t layers);
    void SetTextureOffset(name_t material, vec4 offset);
    
    inline vec3 GetLocation() const { return location; }
    inline quat GetRotation() const { return rotation; }
    inline vec3 GetScale() const { return scale; }
    
    void SetLineDrawingMode(bool enabled);
    
    void SetDirectionaLight(bool enabled);
    void SetRenderDebug(bool enabled);

    void EventHandler(Event &event) { return; }
protected:
    void InsertDrawListEntries();
    void RefreshAABB();
    void Start();

    RenderComponent();
    ~RenderComponent();

    ResourceProxy<Render::Model> model;
    ResourceProxy<Render::Lightmap> lightmap;
    ResourceProxy<Render::Environment> environment;

    std::vector<Render::drawlistentry_t> draw_list_entries;

    Render::Pose* pose = nullptr;

    vec3 location = {0.0f, 0.0f, 0.0f};
    quat rotation = {1.0f, 0.0f, 0.0f, 0.0f};
    vec3 scale = {1.0f, 1.0f, 1.0f};
    vec3 color = {1.0f, 1.0f, 1.0f};
    float opacity = 1.0f;
    uint32_t layer = 0;

    void* aabb_tree_leaf = 0;

    uint32_t render_flags = 0;
};

}

#endif // TRAM_SDK_COMPONENTS_RENDERCOMPONENT_H