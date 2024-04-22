// TRAMWAY DRIFT AND DUNGEON EXPLORATION SIMULATOR 2022
// All rights reserved.

#include <render/api.h>
#include <components/render.h>
#include <components/animation.h>

#include <framework/entity.h>

#include <render/aabb.h>

namespace tram {
using namespace tram::Render;
using namespace tram::Render::API;

template <> Pool<RenderComponent> PoolProxy<RenderComponent>::pool ("render component pool", 250, false);

/// Set the model that the component will render.
/// If the model is not already loaded, then it will be added to loader queue
/// and the component will start when it the loading is complete.
void RenderComponent::SetModel (name_t name) {
    model = Render::Model::Find(name);
    
    if (is_ready) {
        for (auto entry : draw_list_entries) {
            if (entry) {
                RemoveDrawListEntry(entry);
            }
        }
        
        InsertDrawListEntries();
        RefreshAABB();
    }
};

/// Sets the lightmap for the model.
/// Lightmaps are rendered only for static models, so setting a lightmap for
/// a dynamic model will do nothing.
void RenderComponent::SetLightmap (name_t name) {
    lightmap = Render::Material::Find(name);
    
    if (is_ready) {
        for (auto entry : draw_list_entries) {
            if (entry) {
                Render::API::SetLightmap(entry, lightmap ? lightmap->GetTexture() : 0);
            }
        }
    }
};

/// Links an AnimationComponent.
/// This needs to be set, so that the model can be rendered with the animations
/// played by the AnimationComponent.
/// Alternatively, this can be set to a nullptr, if no animations are to be played.
/// This affects only dynamic models, static models don't play animations.
void RenderComponent::SetArmature (AnimationComponent* armature) {
    pose = armature->GetPose();
    
    if (!pose) {
        pose = BLANK_POSE;
    }
    
    if (is_ready) {
        for (auto entry : draw_list_entries) {
            if (entry) {
                Render::API::SetPose(entry, pose);
            }
        }
    }
};

RenderComponent::RenderComponent() : model(this), lightmap(this) {
    render_flags = FLAG_RENDER | FLAG_DRAW_INDEXED;
}

RenderComponent::~RenderComponent() {
    assert(is_ready);
    is_ready = false;
    
    for (auto entry : draw_list_entries) {
        if (entry) {
            Render::API::RemoveDrawListEntry(entry);
        }
    }
    
    if (aabb_tree_leaf) {
        AABB::RemoveLeaf(aabb_tree_leaf);
    }
};

/// Sets the world parameters for model rendering.
void RenderComponent::SetWorldParameters (bool interior_lighting) {
    if (interior_lighting) {
        render_flags |= FLAG_INTERIOR_LIGHTING;
    } else {
        render_flags &= ~FLAG_INTERIOR_LIGHTING;
    }
    
    if (is_ready) {
        for (auto entry : draw_list_entries) {
            if (entry) {
                Render::API::SetFlags(entry, render_flags);
            }
        }
    }
}

/// Sets the location of the model.
void RenderComponent::SetLocation(glm::vec3 nlocation){
    location = nlocation;
    
    if (is_ready) {
        for (auto entry : draw_list_entries) {
            if (entry) {
                Render::API::SetMatrix(entry, PositionRotationToMatrix(location, rotation));
            }
        }
        
        RefreshAABB();
    }
}

/// Sets the rotation of the model.
void RenderComponent::SetRotation(glm::quat nrotation){
    rotation = nrotation;
    
    if (is_ready) {
        for (auto entry : draw_list_entries) {
            if (entry) {
                Render::API::SetMatrix(entry, PositionRotationToMatrix(location, rotation));
            }
        }
        
        RefreshAABB();
    }
}

void RenderComponent::Start(){
    assert(!is_ready);

    InsertDrawListEntries();
    RefreshAABB();
    
    is_ready = true;
}

void RenderComponent::RefreshAABB() {
    if (aabb_tree_leaf) {
        AABB::RemoveLeaf(aabb_tree_leaf);
    }
    
    aabb_tree_leaf = AABB::InsertLeaf(this, location, rotation);
}

void RenderComponent::InsertDrawListEntries() {
    if (!pose) {
        pose = BLANK_POSE;
    }
    
    auto& index_ranges = model->GetIndexRanges();
    
    for (size_t i = 0; i < index_ranges.size(); i++) {
        drawlistentry_t entry = Render::API::InsertDrawListEntry();
    
        texturehandle_t textures[15];
        vec4 colors[15];
        float specular_weights[15];
        float specular_powers[15];
        for (uint32_t j = 0; j < index_ranges[i].material_count; j++) {
            textures[j] = model->GetMaterials()[index_ranges[i].materials[j]]->GetTexture();
            colors[j] = vec4(model->GetMaterials()[index_ranges[i].materials[j]]->GetColor(), 1.0f);
            specular_weights[j] = model->GetMaterials()[index_ranges[i].materials[j]]->GetSpecularWeight();
            specular_powers[j] = model->GetMaterials()[index_ranges[i].materials[j]]->GetSpecularPower();
        }

        Render::API::SetDrawListVertexArray(entry, model->GetVertexArray());
        Render::API::SetDrawListTextures(entry, index_ranges[i].material_count, textures);
        Render::API::SetDrawListColors(entry, index_ranges[i].material_count, colors);
        Render::API::SetDrawListSpecularities(entry, index_ranges[i].material_count, specular_weights, specular_powers);
        Render::API::SetDrawListShader(entry, model->GetVertexFormat(), index_ranges[i].material_type);
        Render::API::SetDrawListIndexRange(entry, index_ranges[i].index_offset, index_ranges[i].index_length);

        Render::API::SetLightmap(entry, lightmap ? lightmap->GetTexture() : 0);
        Render::API::SetFlags(entry, render_flags);

        Render::API::SetMatrix(entry, PositionRotationToMatrix(location, rotation));

        Render::API::SetPose(entry, pose);
        
        draw_list_entries [i] = entry;
    }
}

}
