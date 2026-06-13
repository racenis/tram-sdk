// Tramway Drifting and Dungeon Exploration Simulator SDK Runtime

#include <render/api.h>
#include <components/render.h>
#include <components/animation.h>

#include <framework/entity.h>

#include <render/scene.h>
#include <render/light.h>

#include <config.h>

/**
 * @class tram::RenderComponent components/render.h <components/render.h>
 * 
 * @fn tram::RenderComponent::GetModel
 * @return Pointer to a model, or `nullptr` if it hasn't been set.
 * 
 * @fn tram::RenderComponent::GetLightmap
 * @return Pointer to a lightmap, or `nullptr` if it hasn't been set.
 * 
 * @fn tram::RenderComponent::GetLocation
 * @return Position of the component.
 * 
 * @fn tram::RenderComponent::GetRotation
 * @return Rotation of the component.
 * 
 * @fn tram::RenderComponent::GetScale
 * @return Scale of the component.
 * 
 * Renders a Model.
 * @see https://racenis.github.io/tram-sdk/documentation/components/render.html
 */

namespace tram {

using namespace tram::Render;
using namespace tram::Render::API;

template <> Pool<RenderComponent> PoolProxy<RenderComponent>::pool("RenderComponent pool", COMPONENT_LIMIT_RENDER);
template <> void Component<RenderComponent>::init() { ptr = RenderComponent::Make(); }
template <> void Component<RenderComponent>::yeet() { RenderComponent::Yeet(ptr); }

/// Set the model that the component will render.
/// If the model is not already loaded, then it will be added to loader queue
/// and the component will start when it the loading is complete.
void RenderComponent::SetModel(name_t name) {
    if (is_ready) {
        Log(Severity::WARNING, System::RENDER, "Initialized RenderComponents cannot accept models! Ignoring RenderComponent::SetModel() call.");
        return;
    }
    
    model = Render::Model::Find(name);
};

/// Sets the lightmap for the model.
/// @deprecated Use SetLightmap(Lightmap*) method instead.
void RenderComponent::SetLightmap(name_t name) {
    SetLightmap(Render::Lightmap::Find(name));
};

/// Sets the lightmap for the model.
/// Lightmaps are rendered only for static models, so setting a lightmap for
/// a dynamic model have no effect.
/// @param lightmap Pointer to a lightmap.
void RenderComponent::SetLightmap(Lightmap* lightmap) {
    if (environment) {
        Log(Severity::WARNING, System::RENDER, "RenderComponent already has an environment map! Ignoring RenderComponent::SetLightmap() call.");
        return;
    }
    
    if (is_ready) {
        Log(Severity::WARNING, System::RENDER, "Initialized RenderComponents cannot accept lightmaps! Ignoring RenderComponent::SetLightmap() call.");
        return;
    }
    
    this->lightmap = lightmap;
};

/// Sets the environment map for the model.
/// If calling this method after the component has been initialized, the
/// evironment map needs to be have already been loaded.
/// Before initialization finishes, the component will instead request the load
/// of the map resource and will wait until it is ready.
/// This method can also be used to clear the environment map.
/// @param environment Pointer to environment map to set or `nullptr`.
void RenderComponent::SetEnvironmentMap(Render::Environment* environment) {
    if (lightmap) {
        Log(Severity::WARNING, System::RENDER, "RenderComponent already has a lightmap! Ignoring RenderComponent::SetEnvironmentMap() call.");
        return;
    }
    
    if (!is_ready) {
        this->environment = environment;
        return;
    }
    
    if (!environment) {
        for (auto entry : draw_list_entries) {
            if (!entry.generic) continue;
            Render::API::SetEnvironmentMap(entry, texturearray_t {});
        }
        return;
    }
    
    if (environment->GetStatus() != Resource::READY) {
        Log(Severity::WARNING, System::RENDER, "Initialized RenderComponents can only accept loaded environment maps! Ignoring RenderComponent::SetEnvironmentMap() call");
        return;
    }
    
    for (auto entry : draw_list_entries) {
        if (!entry.generic) continue;
        Render::API::SetEnvironmentMap(entry, environment->GetTexture() );
    }    
}

/// Links an AnimationComponent.
/// @deprecated Use SetPose(AnimationComponent*) instead.
void RenderComponent::SetArmature(AnimationComponent* armature) {
    SetPose(armature);
}

/// Links an AnimationComponent.
/// This needs to be set so that the model can be rendered with the animations
/// played by the AnimationComponent.
/// Alternatively, this can also be set to a nullptr if no animations are to be played.
/// This affects only dynamic models, static models cannot play animations.
void RenderComponent::SetPose(AnimationComponent* armature){
    this->pose = armature ? armature->GetPose() : nullptr;

    if (is_ready) {
        for (auto entry : draw_list_entries) {
            if (!entry.generic) continue;
            Render::API::SetPose(entry, pose);
        }
    }
};

RenderComponent::RenderComponent() : model(this), lightmap(this), environment(this) {
    render_flags = FLAG_RENDER | FLAG_DRAW_INDEXED;
}

RenderComponent::~RenderComponent() {
    is_ready = false;
    
    for (auto entry : draw_list_entries) {
        if (entry.generic) {
            Render::API::RemoveDrawListEntry(entry);
        }
    }
    
    if (aabb_tree_leaf) {
        AABB::RemoveLeaf(aabb_tree_leaf);
    }
};

/// Sets the directional light for model rendering.
/// Enables/disables directional lighting for a model. Setup the scene's
/// directional lighting using `Render::SetSunDirection()` and
/// `Render::SetSunColor()` functions.
/// @param True if model should be rendered with the directional light, false
///        otherwise.
void RenderComponent::SetDirectionaLight(bool enabled) {
    if (!enabled) {
        render_flags |= FLAG_NO_DIRECTIONAL;
    } else {
        render_flags &= ~FLAG_NO_DIRECTIONAL;
    }
    
    if (is_ready) {
        for (auto entry : draw_list_entries) {
            if (entry.generic) {
                Render::API::SetFlags(entry, render_flags);
            }
        }
    }
}

/// Sets render debugging.
/// If set to false, the 3D model will not have debug text for it when the
/// render debug text rendering is turned on.
void RenderComponent::SetRenderDebug(bool enabled) {
    if (!enabled) {
        render_flags |= FLAG_NO_DEBUG;
    } else {
        render_flags &= ~FLAG_NO_DEBUG;
    }
    
    if (is_ready) {
        for (auto entry : draw_list_entries) {
            if (entry.generic) {
                Render::API::SetFlags(entry, render_flags);
            }
        }
    }
}

/// Sets the location of the model.
void RenderComponent::SetLocation(vec3 nlocation){
    location = nlocation;
    
    if (is_ready) {
        for (auto entry : draw_list_entries) {
            if (entry.generic) {
                Render::API::SetMatrix(entry, PositionRotationScaleToMatrix(location, rotation, scale));
                light_t lights[4];
                Render::LightTree::FindLights(location, lights);
                Render::API::SetLights(entry, lights);
                if (!lightmap) {
                    Render::API::SetSphericalHarmonic(entry, LightGraph::LookupHarmonic(location, -1));
                }
            }
        }
        
        RefreshAABB();
    }
}

/// Sets the rotation of the model.
void RenderComponent::SetRotation(quat nrotation){
    rotation = nrotation;
    
    if (is_ready) {
        for (auto entry : draw_list_entries) {
            if (entry.generic) {
                Render::API::SetMatrix(entry, PositionRotationScaleToMatrix(location, rotation, scale));
            }
        }
        
        RefreshAABB();
    }
}

/// Sets the scale of the model.
void RenderComponent::SetScale(vec3 scale) {
    this->scale = scale;
    
    if (is_ready) {
        for (auto entry : draw_list_entries) {
            if (entry.generic) {
                Render::API::SetMatrix(entry, PositionRotationScaleToMatrix(location, rotation, scale));
            }
        }
        
        RefreshAABB();
    }
}

/// Sets the render layer of the component.
/// @deprecated Use SetLayers(uint32_t) instead.
void RenderComponent::SetLayer(uint32_t layer) {
    SetLayers(1 << layer);
}

/// Sets the render layers of the rendercomponent.
/// The layers parameter is a bitmask, with the least significant bit
/// representing layer zero and most significant bit representing layer 31.
/// Default layer is zero.
/// At least a single layer must be selected.
/// @param layers Bitmask containing layers in which the model will be drawn.
void RenderComponent::SetLayers(uint32_t layers) {
    if (!layer) {
        Log(Severity::WARNING, System::RENDER, "RenderComponents must belong to at least a single layer! Ignoring RenderComponent::SetLayers() call.");
        return;
    }
    
    // TODO: finish implementation
    this->layer = layers;
    
    if (is_ready) {
        for (auto entry : draw_list_entries) {
            if (!entry.generic) continue;
            Render::API::SetLayer(entry, layer);
        }
    }
}

/// Sets the texture offset of a material.
/// Allows changing the texture UV offset of a material. This can be used to
/// create, e.g. animated textures.
/// The X and Y parameters of the offset are normalized texture space offset
/// coordinates, e.g. you can set them to `(0.0, 0.5)` to offset the texture
/// horizontally by a half of its width.
/// The Z and W parameters are currently ignored and are reserved for future
/// use, so for now set them to `1.0`.
/// @param material Name of the material.
/// @param offset   Special offset vector.
void RenderComponent::SetTextureOffset(name_t material, vec4 offset) {
    if (is_ready) {
        auto& index_ranges = model->GetIndexRanges();
    
        for (size_t i = 0; i < index_ranges.size(); i++) {
            vec4 offsets[15];

            for (uint32_t j = 0; j < index_ranges[i].material_count; j++) {
                if (model->GetMaterials()[index_ranges[i].materials[j]]->GetName() == material) {
                    offsets[j] = offset;
                } else {
                    offsets[j] = vec4(1.0f);
                }
            }

            Render::API::SetDrawListTextureOffsets(draw_list_entries[i], index_ranges[i].material_count, offsets);
        }
    }
}

/// Sets the line drawing mode.
/// If enabled, the model will be drawn as a wireframe.
void RenderComponent::SetLineDrawingMode(bool enabled) {
    if (enabled) {
        render_flags |= FLAG_LINE_FILL_POLY;
    } else {
        render_flags &= ~FLAG_LINE_FILL_POLY;
    }
    
    if (is_ready) {
        for (auto entry : draw_list_entries) {
            if (entry.generic) {
                Render::API::SetFlags(entry, render_flags);
            }
        }
    }
}

/// Sets the scale of the model.
void RenderComponent::SetColor(vec3 color) {
    this->color = color;
    
    if (is_ready) {
        for (auto entry : draw_list_entries) {
            if (entry.generic) {
                Render::API::SetDrawListColor(entry, vec4(color, 1.0f));
            }
        }
    }
}

void RenderComponent::Start() {
    if (!model) {
        Log(Severity::CRITICAL_ERROR, System::RENDER, "Render component doesn't have its model set!");
    }
    
    InsertDrawListEntries();
    RefreshAABB();
    
    is_ready = true;
}

void RenderComponent::RefreshAABB() {
    if (aabb_tree_leaf) {
        AABB::RemoveLeaf(aabb_tree_leaf);
    }
    
    aabb_tree_leaf = AABB::InsertLeaf(this, location, rotation, scale);
}

void RenderComponent::InsertDrawListEntries() {
    auto& index_ranges = model->GetIndexRanges();
    
    for (size_t i = 0; i < index_ranges.size(); i++) {
        drawlistentry_t entry = Render::API::InsertDrawListEntry();
    
        texturehandle_t textures[15];
        material_t materials[15];
        vec4 colors[15];
        for (uint32_t j = 0; j < index_ranges[i].material_count; j++) {
            materials[j] = model->GetMaterials()[index_ranges[i].materials[j]]->GetMaterial();
            textures[j] = model->GetMaterials()[index_ranges[i].materials[j]]->GetTexture();
            colors[j] = vec4(model->GetMaterials()[index_ranges[i].materials[j]]->GetColor() * color, 1.0f);
        }

        light_t lights[4];
        Render::LightTree::FindLights(location, lights);
        Render::API::SetLights(entry, lights);
        if (!lightmap) {
            Render::API::SetSphericalHarmonic(entry, LightGraph::LookupHarmonic(location, -1));
        }

        Render::API::SetDrawListVertexArray(entry, model->GetVertexArray());
        Render::API::SetDrawListIndexArray(entry, model->GetIndexArray());
        Render::API::SetDrawListMaterials(entry, index_ranges[i].material_count, materials);
        Render::API::SetDrawListColor(entry, vec4(color, 1.0f));
        const bool found_shader = Render::API::SetDrawListShader(entry, model->GetVertexFormat(), index_ranges[i].material_type);
        Render::API::SetDrawListIndexRange(entry, index_ranges[i].index_offset, index_ranges[i].index_length);

        Render::API::SetLightmap(entry, lightmap ? lightmap->GetTexture() : texturearray_t {});
        Render::API::SetEnvironmentMap(entry, environment ? environment->GetTexture() : texturearray_t {});
        Render::API::SetFlags(entry, render_flags);
        Render::API::SetLayer(entry, layer);

        Render::API::SetMatrix(entry, PositionRotationScaleToMatrix(location, rotation, scale));

        Render::API::SetPose(entry, pose);
        
        const vec3 aabb_min = model->GetAABBMin();
        const vec3 aabb_max = model->GetAABBMax();
        if (glm::distance(aabb_min, aabb_max) > 0.001f) {
            render_flags |= FLAG_USE_AABB;
            Render::API::SetDrawListAABB(entry, aabb_min, aabb_max);
            Render::API::SetFlags(entry, render_flags);
        }
        
        Render::API::SetFadeDistance(entry, model->GetNearDistance(), model->GetFarDistance());
        
        if (!found_shader) {
            Log(Severity::WARNING, System::RENDER, "Can't find shader for the following {} materials:", index_ranges.size());
            for (uint32_t j = 0; j < index_ranges[i].material_count; j++) {
                 Log(Severity::WARNING, System::RENDER, "Material {}: {}", j, model->GetMaterials()[index_ranges[i].materials[j]]->GetName());
            }
        }
        
        draw_list_entries.push_back(entry);
    }
}

/// Creates a new RenderComponent.
RenderComponent* RenderComponent::Make() {
    RenderComponent* ptr = PoolProxy<RenderComponent>::GetPool().allocate();
    new(ptr) RenderComponent();
    return ptr;
}

/// Deletes an RenderComponent.
void RenderComponent::Yeet(RenderComponent* component) {
    component->~RenderComponent();
    PoolProxy<RenderComponent>::GetPool().deallocate(component);
}

}
