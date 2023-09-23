// TRAMWAY DRIFT AND DUNGEON EXPLORATION SIMULATOR 2022
// All rights reserved.

#ifndef COMPONENTS_RENDERCOMPONENT_H
#define COMPONENTS_RENDERCOMPONENT_H

#include <render/model.h>

namespace tram {

class AnimationComponent;

class RenderComponent : public EntityComponent {
public:
    RenderComponent();
    ~RenderComponent();
    inline Render::Model* GetModel(){return model.get();};

    inline name_t GetLightmap(){ return (lightmap.get() == nullptr) ? 0 : lightmap->GetName();};

    void SetModel(name_t name);
    void SetLightmap(name_t name);
    void SetArmature(AnimationComponent* armature);

    void Start();

    vec3 GetLocation() const { return location; }
    quat GetRotation() const { return rotation; }

    void SetLocation(glm::vec3 nlocation);
    void SetRotation(glm::quat nrotation);
    void SetWorldParameters (bool interior_lighting);

    void EventHandler(Event &event){return;}
protected:
    void InsertDrawListEntries();
    void RefreshAABB();

    ResourceProxy<Render::Model> model;
    ResourceProxy<Render::Material> lightmap;

    Render::drawlistentry_t draw_list_entries [3] = {nullptr};

    Render::Pose* pose = nullptr;

    vec3 location = {0.0f, 0.0f, 0.0f};
    quat rotation = {1.0f, 0.0f, 0.0f, 0.0f};

    void* aabb_tree_leaf = 0;

    uint32_t render_flags;
};

}

#endif // COMPONENTS_RENDERCOMPONENT_H