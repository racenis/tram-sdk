// TRAMWAY DRIFT AND DUNGEON EXPLORATION SIMULATOR 2022
// All rights reserved.
//
// RENDERCOMPONENT.CPP -- implementation of RenderComponent

#include <render/renderer.h>
#include <components/rendercomponent.h>

namespace Core {
    using namespace Core::Render;
    
    void RenderComponent::SetModel(uint64_t name){
        model.SetResource(Render::Model::Find(name));
        
        if (is_ready) {
            OpenGL::RemoveDrawListEntry(draw_list_entry);
            InsertDrawListEntry();
        }
    };

    void RenderComponent::SetLightmap(uint64_t name){
        lightmap.SetResource(Render::Material::Find(name));
        
        if (is_ready) {
            OpenGL::SetLightmap(draw_list_entry, lightmap ? lightmap->GetTexture() : 0);
        }
    };

    void RenderComponent::SetPose(Render::Pose* newPose){
        pose = newPose == nullptr ? Render::poseList.begin().ptr : newPose; // make a global variable BLANK_POSE or something like that? instead of poseList.begin()?
        
        if (is_ready) {
            OpenGL::SetPose(draw_list_entry, pose);
        }
    };

    void RenderComponent::Init() {
        assert(!is_ready);
        if (resources_waiting == 0) Start();
    };
    
    void RenderComponent::Uninit(){
        assert(is_ready);
        is_ready = false;
        
        OpenGL::RemoveDrawListEntry(draw_list_entry);
    };
    
    void RenderComponent::SetCellParams (bool isInteriorLight){
        isInterior = isInteriorLight;
        if (is_ready) {
            if (isInteriorLight) {
                OpenGL::SetFlags(draw_list_entry, OpenGL::GetFlags(draw_list_entry) | FLAG_INTERIOR_LIGHTING);
            } else {
                OpenGL::SetFlags(draw_list_entry, OpenGL::GetFlags(draw_list_entry) & (~FLAG_INTERIOR_LIGHTING));
            }
        }
    }
    
    void RenderComponent::UpdateLocation(glm::vec3 nlocation){
        location = nlocation;
        
        if (is_ready) {
            OpenGL::SetLocation(draw_list_entry, location);
            
            uint32_t lights[4];
            lightTree.FindNearest(lights, location.x, location.y, location.z);
            OpenGL::SetLights(draw_list_entry, lights);
        }
    }

    void RenderComponent::UpdateRotation(glm::quat nrotation){
        rotation = nrotation;
        
        if (is_ready) {
            OpenGL::SetRotation(draw_list_entry, rotation);
        }
    }

    void RenderComponent::Start(){
        assert(!is_ready);

        InsertDrawListEntry();
        
        is_ready = true;
    }

    void RenderComponent::InsertDrawListEntry() {
        draw_list_entry = OpenGL::InsertDrawListEntry(model.GetResource());
        
        OpenGL::SetLightmap(draw_list_entry, lightmap ? lightmap->GetTexture() : 0);
        if (isInterior) OpenGL::SetFlags(draw_list_entry, OpenGL::GetFlags(draw_list_entry) | FLAG_INTERIOR_LIGHTING);
        
        OpenGL::SetLocation(draw_list_entry, location);
        OpenGL::SetRotation(draw_list_entry, rotation);
        
        uint32_t lights[4];
        lightTree.FindNearest(lights, location.x, location.y, location.z);
        OpenGL::SetLights(draw_list_entry, lights);
        
        OpenGL::SetPose(draw_list_entry, pose);
    }
}