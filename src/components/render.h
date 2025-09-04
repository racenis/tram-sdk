// Tramway Drifting and Dungeon Exploration Simulator SDK Runtime

#ifndef TRAM_SDK_COMPONENTS_RENDERCOMPONENT_H
#define TRAM_SDK_COMPONENTS_RENDERCOMPONENT_H

#include <render/model.h>

namespace tram {

class AnimationComponent;

class RenderComponent : public EntityComponent {
public:
    RenderComponent();
    ~RenderComponent();
    inline Render::Model* GetModel() { return model.get(); }

    inline name_t GetLightmap() { return (lightmap.get() == nullptr) ? 0 : lightmap->GetName();};

    void SetModel(name_t name);
    void SetModel(Render::Model* model) { this->model = model; }
    void SetLightmap(name_t name);
    void SetEnvironmentMap(Render::Material* material);
    void SetArmature(AnimationComponent* armature);

    void Start();

    vec3 GetLocation() const { return location; }
    quat GetRotation() const { return rotation; }
    vec3 GetScale() const { return scale; }

    void SetLocation(vec3 nlocation);
    void SetRotation(quat nrotation);
    void SetScale(vec3 scale);
    void SetColor(vec3 color);
    void SetLayer(uint32_t layer);
    void SetTextureOffset(name_t material, vec4 offset);
    
    void SetDirectionaLight(bool enabled);
    void SetRenderDebug(bool enabled);

    void EventHandler(Event &event) { return; }
protected:
    void InsertDrawListEntries();
    void RefreshAABB();

    ResourceProxy<Render::Model> model;
    ResourceProxy<Render::Material> lightmap;
    ResourceProxy<Render::Material> environmentmap;

    std::vector<Render::drawlistentry_t> draw_list_entries;

    Render::Pose* pose = nullptr;

    vec3 location = {0.0f, 0.0f, 0.0f};
    quat rotation = {1.0f, 0.0f, 0.0f, 0.0f};
    vec3 scale = {1.0f, 1.0f, 1.0f};
    vec3 color = {1.0f, 1.0f, 1.0f};
    uint32_t layer = 0;

    void* aabb_tree_leaf = 0;

    uint32_t render_flags = 0;
};

}

#endif // TRAM_SDK_COMPONENTS_RENDERCOMPONENT_H