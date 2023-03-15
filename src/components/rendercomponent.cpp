// TRAMWAY DRIFT AND DUNGEON EXPLORATION SIMULATOR 2022
// All rights reserved.

#include <render/renderer.h>
#include <components/rendercomponent.h>
#include <components/armaturecomponent.h>

namespace tram {
    using namespace tram::Render;
    
    /// Set the model that the component will render.
    /// If the model is not already loaded, then it will be added to loader queue
    /// and the component will start when it the loading is complete.
    void RenderComponent::SetModel(name_t name){
        model = Render::Model::Find(name);
        
        if (is_ready) {
            RemoveDrawListEntry(draw_list_entry);
            InsertDrawListEntry();
        }
    };

    /// Sets the lightmap for the model.
    /// Lightmaps are rendered only for static models, so setting a lightmap for
    /// a dynamic model will do nothing.
    void RenderComponent::SetLightmap(name_t name){
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
    void RenderComponent::SetArmature(ArmatureComponent* armature){
        pose = armature->GetPose();
        
        // maybe make a global variable BLANK_POSE or something like that? instead of poseList.begin()?
        if (!pose) {
            pose = Render::poseList.begin().ptr;
        }
        
        if (is_ready) {
            Render::SetPose(draw_list_entry, pose);
        }
    };
    
    RenderComponent::RenderComponent() : model(this), lightmap(this){
        render_flags = FLAG_RENDER | FLAG_DRAW_INDEXED;
    }
    
    RenderComponent::~RenderComponent(){
        assert(is_ready);
        is_ready = false;
        
        RemoveDrawListEntry(draw_list_entry);
    };
    
    void RenderComponent::SetWorldParameters(bool interior_lighting){
        if (interior_lighting) {
            render_flags |= FLAG_INTERIOR_LIGHTING;
        } else {
            render_flags &= ~FLAG_INTERIOR_LIGHTING;
        }
        
        if (is_ready) {
            SetFlags(draw_list_entry, render_flags);
        }
    }
    
    void RenderComponent::UpdateLocation(glm::vec3 nlocation){
        location = nlocation;
        
        if (is_ready) {
            SetLocation(draw_list_entry, location);
            
            uint32_t lights[4];
            lightTree.FindNearest(lights, location.x, location.y, location.z);
            SetLights(draw_list_entry, lights);
        }
    }

    void RenderComponent::UpdateRotation(glm::quat nrotation){
        rotation = nrotation;
        
        if (is_ready) {
            SetRotation(draw_list_entry, rotation);
        }
    }

    void RenderComponent::Start(){
        assert(!is_ready);

        InsertDrawListEntry();
        
        is_ready = true;
    }

    void RenderComponent::InsertDrawListEntry() {
        draw_list_entry = InsertDrawListEntryFromModel(model.get());
        
        Render::SetLightmap(draw_list_entry, lightmap ? lightmap->GetTexture() : 0);
        Render::SetFlags(draw_list_entry, render_flags);
        //if (isInterior) SetFlags(draw_list_entry, GetFlags(draw_list_entry) | FLAG_INTERIOR_LIGHTING);
        
        SetLocation(draw_list_entry, location);
        SetRotation(draw_list_entry, rotation);
        
        uint32_t lights[4];
        lightTree.FindNearest(lights, location.x, location.y, location.z);
        SetLights(draw_list_entry, lights);
        
        // this sets the pose to the default identity matrix pose
        if (!pose) {
            pose = Render::poseList.begin().ptr;
        }
        
        Render::SetPose(draw_list_entry, pose);
    }
}