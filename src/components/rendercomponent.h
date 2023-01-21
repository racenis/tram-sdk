// TRAMWAY DRIFT AND DUNGEON EXPLORATION SIMULATOR 2022
// All rights reserved.

#ifndef COMPONENTS_RENDERCOMPONENT_H
#define COMPONENTS_RENDERCOMPONENT_H

#include <render/model.h>

namespace Core {
    class RenderComponent : public EntityComponent {
    public:
        RenderComponent() : model(this), lightmap(this){}
        ~RenderComponent();
        inline name_t GetModel(){return model->GetName();};

        inline name_t GetLightmap(){ return (lightmap.get() == nullptr) ? 0 : lightmap->GetName();};

        void SetModel(name_t name);

        void SetLightmap(name_t name);

        void SetPose(Render::Pose* newPose);

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

#endif // COMPONENTS_RENDERCOMPONENT_H