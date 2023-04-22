// TRAMWAY DRIFT AND DUNGEON EXPLORATION SIMULATOR 2022
// All rights reserved.

#include <render/api.h>
#include <components/rendercomponent.h>
#include <components/armaturecomponent.h>

#include <framework/entity.h>

#include <templates/aabb.h>

namespace tram {
using namespace tram::Render;

template <> Pool<RenderComponent> PoolProxy<RenderComponent>::pool ("render component pool", 250, false);

static AABBTree rendertree;

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
                Render::SetLightmap(entry, lightmap ? lightmap->GetTexture() : 0);
            }
        }
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
        for (auto entry : draw_list_entries) {
            if (entry) {
                Render::SetPose(entry, pose);
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
            Render::RemoveDrawListEntry(entry);
        }
    }
    
    if (aabb_tree_key) {
        rendertree.RemoveLeaf(aabb_tree_key);
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
                Render::SetFlags(entry, render_flags);
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
                Render::SetLocation(entry, location);
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
                Render::SetRotation(entry, rotation);
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
    vec3 min = model->GetAABBMin();
    vec3 max = model->GetAABBMax();
    
    vec3 extents[8] = {
        {min.x, min.y, min.z},
        {max.x, min.y, min.z},
        {min.x, max.y, min.z},
        {min.x, min.y, max.z},
        {max.x, max.y, min.z},
        {max.x, min.y, max.z},
        {max.x, max.y, max.z},
        {min.x, max.y, max.z}
    };
    
    for (auto& extent : extents) {
        extent = rotation * extent;
    }
    
    min = extents[0];
    max = extents[0];
    
    for (auto& extent : extents) {
        min = AABBTree::MergeAABBMin(min, extent);
        max = AABBTree::MergeAABBMax(max, extent);
    }

    min += location;
    max += location;

    //AddLineAABB(min, max, {0.0f, 0.0f, 0.0f}, {1.0f, 0.0f, 0.0f, 0.0f}, COLOR_CYAN);
    
    //if (aabb_tree_key) return;
    if (aabb_tree_key) rendertree.RemoveLeaf(aabb_tree_key);
    
    aabb_tree_key = rendertree.InsertLeaf(this - PoolProxy<RenderComponent>::GetPool().begin().ptr, min, max);
}

void RenderComponent::InsertDrawListEntries() {
    if (!pose) {
        pose = BLANK_POSE;
    }
    
    auto& index_ranges = model->GetIndexRanges();
    
    for (size_t i = 0; i < index_ranges.size(); i++) {
        drawlistentry_t entry = Render::InsertDrawListEntry();
    
        texturehandle_t textures [15];
        for (uint32_t j = 0; j < index_ranges[i].material_count; j++) {
            textures [j] = model->GetMaterials()[index_ranges[i].materials[j]]->GetTexture();
        }

        Render::SetDrawListVertexArray(entry, model->GetVertexArray());
        Render::SetDrawListTextures(entry, index_ranges[i].material_count, textures);
        Render::SetDrawListShader(entry, model->GetVertexFormat(), index_ranges[i].material_type);
        Render::SetDrawListIndexRange(entry, index_ranges[i].index_offset, index_ranges[i].index_length);

        Render::SetLightmap(entry, lightmap ? lightmap->GetTexture() : 0);
        Render::SetFlags(entry, render_flags);

        Render::SetLocation(entry, location);
        Render::SetRotation(entry, rotation);

        Render::SetPose(entry, pose);
        
        draw_list_entries [i] = entry;
    }
}

static void DrawAABBNodeChildren (uint32_t node_id) {
    const auto& node = rendertree.nodes[node_id];
    
    if (node.IsLeaf()) {
        AddLineAABB(node.min, node.max, {0.0f, 0.0f, 0.0f}, {1.0f, 0.0f, 0.0f, 0.0f}, COLOR_CYAN);
    } else {
        DrawAABBNodeChildren(node.left);
        DrawAABBNodeChildren(node.right);
        
        if (node_id == 0) {
            AddLineAABB(node.min, node.max, {0.0f, 0.0f, 0.0f}, {1.0f, 0.0f, 0.0f, 0.0f}, COLOR_RED);
        } else {
            AddLineAABB(node.min, node.max, {0.0f, 0.0f, 0.0f}, {1.0f, 0.0f, 0.0f, 0.0f}, COLOR_PINK);
        }
    }
}

void RenderComponent::DrawAllAABB() {
    /*for (auto& component : PoolProxy<RenderComponent>::GetPool()) {
        name_t model_name = component.GetModel();
        Model* model_ptr = Model::Find(model_name);
        
        Entity* parent = component.GetParent();
        
        if (!parent) continue;
        
        vec3 parent_position = parent->GetLocation();
        quat parent_rotation = parent->GetRotation();
        
        model_ptr->DrawAABB(parent_position, parent_rotation);
    }*/
    
    DrawAABBNodeChildren(0);
}

}
