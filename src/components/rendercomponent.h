// TRAMWAY DRIFT AND DUNGEON EXPLORATION SIMULATOR 2022
// All rights reserved.
//
// RENDERCOMPONENT.H -- Model rendering component
// This component will render a single model onto the screen.

#ifndef RENDERCOMPONENT_H
#define RENDERCOMPONENT_H

#include <render.h>

namespace Core {
    class RenderComponent : public EntityComponent {
    public:
        RenderComponent() : model(this), lightmap(this){}
        ~RenderComponent() = default;
        inline name_t GetModel(){return model->GetName();};

        inline name_t GetLightmap(){return (lightmap.GetResource() == nullptr) ? 0 : lightmap->GetName();};

        void SetModel(uint64_t name){
            model.SetResource(Render::Model::Find(name));
        };

        void SetLightmap(uint64_t name){
            lightmap.SetResource(Render::Material::Find(name));
        };

        void SetPose(Render::PoseListObject* newPose){
            pose = newPose == nullptr ? Render::poseList.begin().ptr : newPose;
        };

        void Init(){
            is_ready = false; if(resources_waiting == 0) Start();
        };

        void Uninit();

        void Start();

        void UpdateRenderListObjs();

        void UpdateLocation(glm::vec3 nlocation){
            location = nlocation;
            UpdateRenderListObjs();
        }

        void UpdateRotation(glm::quat nrotation){
            rotation = nrotation;
            UpdateRenderListObjs();
        }

        void SetCellParams (bool isInteriorLight){
            isInterior = isInteriorLight;
        }

        void EventHandler(Event &event){return;}

    protected:
        ResourceProxy<Render::Model> model;
        ResourceProxy<Render::Material> lightmap;

        Render::RenderListObject* rLsObjPtr[7] = {nullptr};

        Render::PoseListObject* pose = nullptr;

        glm::vec3 location;

        glm::quat rotation;

        bool isInterior = false;
    };
}

#endif //RENDERCOMPONENT_H