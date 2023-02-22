// TRAMWAY DRIFT AND DUNGEON EXPLORATION SIMULATOR 2022
// All rights reserved.

#include <render/renderer.h>
#include <components/rendercomponent.h>

namespace Core {
    using namespace Core::Render;
    
    void RenderComponent::SetModel(name_t name){
        model = Render::Model::Find(name);
        
        if (is_ready) {
            RemoveDrawListEntry(draw_list_entry);
            InsertDrawListEntry();
        }
    };

    void RenderComponent::SetLightmap(name_t name){
        lightmap = Render::Material::Find(name);
        
        if (is_ready) {
            Render::SetLightmap(draw_list_entry, lightmap ? lightmap->GetTexture() : 0);
        }
    };

    void RenderComponent::SetPose(Render::Pose* newPose){
        pose = newPose == nullptr ? Render::poseList.begin().ptr : newPose; // make a global variable BLANK_POSE or something like that? instead of poseList.begin()?
        
        if (is_ready) {
            Render::SetPose(draw_list_entry, pose);
        }
    };
    
    RenderComponent::~RenderComponent(){
        assert(is_ready);
        is_ready = false;
        
        RemoveDrawListEntry(draw_list_entry);
    };
    
    void RenderComponent::SetCellParams (bool isInteriorLight){
        isInterior = isInteriorLight;
        if (is_ready) {
            if (isInteriorLight) {
                SetFlags(draw_list_entry, GetFlags(draw_list_entry) | FLAG_INTERIOR_LIGHTING);
            } else {
                SetFlags(draw_list_entry, GetFlags(draw_list_entry) & (~FLAG_INTERIOR_LIGHTING));
            }
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
        if (isInterior) SetFlags(draw_list_entry, GetFlags(draw_list_entry) | FLAG_INTERIOR_LIGHTING);
        
        SetLocation(draw_list_entry, location);
        SetRotation(draw_list_entry, rotation);
        
        uint32_t lights[4];
        lightTree.FindNearest(lights, location.x, location.y, location.z);
        SetLights(draw_list_entry, lights);
        
        Render::SetPose(draw_list_entry, pose);
    }
}