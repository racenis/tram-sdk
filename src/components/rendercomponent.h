// TRAMWAY DRIFT AND DUNGEON EXPLORATION SIMULATOR 2022
// All rights reserved.
//
// RENDERCOMPONENT.H -- Model rendering component
// This component will render a single model onto the screen.

#ifndef RENDERCOMPONENT_H
#define RENDERCOMPONENT_H

#include <render/render.h>

namespace Core {
    class RenderComponent : public EntityComponent {
    public:
        RenderComponent() : model(this), lightmap(this){}
        ~RenderComponent() = default;
        inline name_t GetModel(){return model->GetName();};

        inline name_t GetLightmap(){ return (lightmap.get() == nullptr) ? 0 : lightmap->GetName();};

        void SetModel(uint64_t name);

        void SetLightmap(uint64_t name);

        void SetPose(Render::Pose* newPose);

        void Init();

        void Uninit();

        void Start();

        void UpdateLocation(glm::vec3 nlocation);

        void UpdateRotation(glm::quat nrotation);

        void SetCellParams (bool isInteriorLight);

        void EventHandler(Event &event){return;}

    protected:
        void InsertDrawListEntry();
    
        ResourceProxy<Render::Model> model;
        ResourceProxy<Render::Material> lightmap;

        Render::DrawListEntryHandle draw_list_entry;

        Render::Pose* pose = nullptr;

        glm::vec3 location;
        
        glm::quat rotation;

        bool isInterior = false;
    };
}

#endif //RENDERCOMPONENT_H