// TRAMWAY DRIFT AND DUNGEON EXPLORATION SIMULATOR 2022
// All rights reserved.

#include <render/api.h>
#include <components/rendercomponent.h>
#include <components/armaturecomponent.h>

namespace tram {
using namespace tram::Render;

template <> Pool<RenderComponent> PoolProxy<RenderComponent>::pool ("render component pool", 250, false);

/// Set the model that the component will render.
/// If the model is not already loaded, then it will be added to loader queue
/// and the component will start when it the loading is complete.
void RenderComponent::SetModel (name_t name) {
    model = Render::Model::Find(name);
    
    if (is_ready) {
        RemoveDrawListEntry(draw_list_entry);
        InsertDrawListEntry();
    }
};

/// Sets the lightmap for the model.
/// Lightmaps are rendered only for static models, so setting a lightmap for
/// a dynamic model will do nothing.
void RenderComponent::SetLightmap (name_t name) {
    lightmap = Render::Material::Find(name);
    
    if (is_ready) {
        Render::SetLightmap(draw_list_entry, lightmap ? lightmap->GetTexture() : 0);
    }
};

/// Links an ArmatureComponent.
/// This needs to be set, so that the model can be rendered with the animations
/// played by the ArmatureComponent.
/// Alternatively, this can be set to a nullptr, if no animations are to be played.
/// This affects only dynamic models, static models don't play animations.
void RenderComponent::SetArmature (ArmatureComponent* armature) {
    pose = armature->GetPose();
    
    if (!pose) {
        pose = BLANK_POSE;
    }
    
    if (is_ready) {
        Render::SetPose(draw_list_entry, pose);
    }
};

RenderComponent::RenderComponent() : model(this), lightmap(this) {
    render_flags = FLAG_RENDER | FLAG_DRAW_INDEXED;
}

RenderComponent::~RenderComponent() {
    assert(is_ready);
    is_ready = false;
    
    RemoveDrawListEntry(draw_list_entry);
};

/// Sets the world parameters for model rendering.
void RenderComponent::SetWorldParameters (bool interior_lighting) {
    if (interior_lighting) {
        render_flags |= FLAG_INTERIOR_LIGHTING;
    } else {
        render_flags &= ~FLAG_INTERIOR_LIGHTING;
    }
    
    if (is_ready) {
        SetFlags(draw_list_entry, render_flags);
    }
}

/// Sets the location of the model.
void RenderComponent::SetLocation(glm::vec3 nlocation){
    location = nlocation;
    
    if (is_ready) {
        Render::SetLocation(draw_list_entry, location);
    }
}

/// Sets the rotation of the model.
void RenderComponent::SetRotation(glm::quat nrotation){
    rotation = nrotation;
    
    if (is_ready) {
        Render::SetRotation(draw_list_entry, rotation);
    }
}

void RenderComponent::Start(){
    assert(!is_ready);

    InsertDrawListEntry();
    
    is_ready = true;
}

void RenderComponent::InsertDrawListEntry() {
    draw_list_entry = Render::InsertDrawListEntry();
    
    texturehandle_t textures [15];
    for (uint32_t i = 0; i < model->GetIndexRanges()[0].material_count; i++) {
        textures [i] = model->GetMaterials()[model->GetIndexRanges()[0].materials[i]]->GetTexture();
    }
    
    Render::SetDrawListVertexArray(draw_list_entry, model->GetVertexArray());
    Render::SetDrawListTextures(draw_list_entry, model->GetIndexRanges()[0].material_count, textures);
    Render::SetDrawListShader(draw_list_entry, model->GetVertexFormat(), model->GetIndexRanges()[0].material_type);
    Render::SetDrawListIndexRange(draw_list_entry, model->GetIndexRanges()[0].index_offset, model->GetIndexRanges()[0].index_length);
    
    Render::SetLightmap(draw_list_entry, lightmap ? lightmap->GetTexture() : 0);
    Render::SetFlags(draw_list_entry, render_flags);
    
    Render::SetLocation(draw_list_entry, location);
    Render::SetRotation(draw_list_entry, rotation);
    
    if (!pose) {
        pose = BLANK_POSE;
    }
    
    Render::SetPose(draw_list_entry, pose);
}

}
