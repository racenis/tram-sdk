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
 * Renders a Model.
 * @see https://racenis.github.io/tram-sdk/documentation/components/render.html
 */

namespace tram {

using namespace tram::Render;
using namespace tram::Render::API;

template <> Pool<RenderComponent> PoolProxy<RenderComponent>::pool("RenderComponent pool", COMPONENT_LIMIT_RENDER);
template <> void Component<RenderComponent>::init() { ptr = PoolProxy<RenderComponent>::New(); }
template <> void Component<RenderComponent>::yeet() { PoolProxy<RenderComponent>::Delete(ptr); }

/// Set the model that the component will render.
/// If the model is not already loaded, then it will be added to loader queue
/// and the component will start when it the loading is complete.
void RenderComponent::SetModel(name_t name) {
    model = Render::Model::Find(name);
    
    /*if (is_ready) {
        for (auto entry : draw_list_entries) {
            if (entry.generic) {
                RemoveDrawListEntry(entry);
            }
        }
        
        draw_list_entries.clear();
        
        InsertDrawListEntries();
        RefreshAABB();
    }*/
};

/// Sets the lightmap for the model.
/// Lightmaps are rendered only for static models, so setting a lightmap for
/// a dynamic model will do nothing.
void RenderComponent::SetLightmap(name_t name) {
    lightmap = Render::Material::Make(name, MATERIAL_LIGHTMAP);
    
    /*if (is_ready) {
        for (auto entry : draw_list_entries) {
            if (entry.generic) {
                Render::API::SetLightmap(entry, lightmap ? lightmap->GetTexture() : texturehandle_t {});
            }
        }
    }*/
};

/// Sets the environment map for the model.
void RenderComponent::SetEnvironmentMap(Render::Material* material) {
    environmentmap = material;
    //std::cout << "setting:" << material << std::endl;
    //if (material != environmentmap.get()) std::cout << "newnenwnenwenwnew\n\n\n\n\n\n\n\n" << std::endl;
    
    if (is_ready && material && material->GetStatus() == Resource::READY) {
        for (auto entry : draw_list_entries) {
            if (entry.generic) {
                Render::API::SetEnvironmentMap(entry, environmentmap ? environmentmap->GetTexture() : texturehandle_t {});
            }
        }
    }
    
    
    
}

/// Links an AnimationComponent.
/// This needs to be set, so that the model can be rendered with the animations
/// played by the AnimationComponent.
/// Alternatively, this can be set to a nullptr, if no animations are to be played.
/// This affects only dynamic models, static models don't play animations.
void RenderComponent::SetArmature(AnimationComponent* armature) {
    if (armature) {
        pose = armature->GetPose();
    } else {
        pose = nullptr;
    }
    
    if (is_ready) {
        for (auto entry : draw_list_entries) {
            if (entry.generic) {
                Render::API::SetPose(entry, pose);
            }
        }
    }
};

RenderComponent::RenderComponent() : model(this), lightmap(this), environmentmap(this) {
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

/// Sets the world parameters for model rendering.
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

void RenderComponent::SetLayer(uint32_t layer) {
    this->layer = layer;
    
    if (is_ready) {
        for (auto entry : draw_list_entries) {
            if (entry.generic) {
                Render::API::SetLayer(entry, layer);
            }
        }
    }
}

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
        auto& index_ranges = model->GetIndexRanges();
    
        for (size_t i = 0; i < index_ranges.size(); i++) {
            vec4 colors[15];

            for (uint32_t j = 0; j < index_ranges[i].material_count; j++) {
                colors[j] = vec4(model->GetMaterials()[index_ranges[i].materials[j]]->GetColor() * color, 1.0f);
            }

            Render::API::SetDrawListColors(draw_list_entries[i], index_ranges[i].material_count, colors);
        }
    }
}

void RenderComponent::Start() {
    if (is_ready) {
        for (auto entry : draw_list_entries) {
            if (entry.generic) {
                Render::API::RemoveDrawListEntry(entry);
            }
        }
        
        draw_list_entries.clear();
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
    /*if (!pose) {
        pose = BLANK_POSE;
    }*/
    
    auto& index_ranges = model->GetIndexRanges();
    
    for (size_t i = 0; i < index_ranges.size(); i++) {
        drawlistentry_t entry = Render::API::InsertDrawListEntry();
    
        texturehandle_t textures[15];
        material_t materials[15];
        vec4 colors[15];
        //float specular_weights[15];
        //float specular_exponents[15];
        //float specular_transparencies[15];
        for (uint32_t j = 0; j < index_ranges[i].material_count; j++) {
            materials[j] = model->GetMaterials()[index_ranges[i].materials[j]]->GetMaterial();
            textures[j] = model->GetMaterials()[index_ranges[i].materials[j]]->GetTexture();
            colors[j] = vec4(model->GetMaterials()[index_ranges[i].materials[j]]->GetColor() * color, 1.0f);
            //specular_weights[j] = model->GetMaterials()[index_ranges[i].materials[j]]->GetSpecularWeight();
            //specular_exponents[j] = model->GetMaterials()[index_ranges[i].materials[j]]->GetSpecularExponent();
            //specular_transparencies[j] = model->GetMaterials()[index_ranges[i].materials[j]]->GetSpecularTransparency();
        }

        light_t lights[4];
        Render::LightTree::FindLights(location, lights);
        Render::API::SetLights(entry, lights);

        Render::API::SetDrawListVertexArray(entry, model->GetVertexArray());
        Render::API::SetDrawListIndexArray(entry, model->GetIndexArray());
        Render::API::SetDrawListMaterials(entry, index_ranges[i].material_count, materials);
        //Render::API::SetDrawListTextures(entry, index_ranges[i].material_count, textures);
        Render::API::SetDrawListColors(entry, index_ranges[i].material_count, colors);
        //Render::API::SetDrawListSpecularities(entry, index_ranges[i].material_count, specular_weights, specular_exponents, specular_transparencies);
        const bool found_shader = Render::API::SetDrawListShader(entry, model->GetVertexFormat(), index_ranges[i].material_type);
        Render::API::SetDrawListIndexRange(entry, index_ranges[i].index_offset, index_ranges[i].index_length);

        Render::API::SetLightmap(entry, lightmap ? lightmap->GetTexture() : texturehandle_t {});
        Render::API::SetEnvironmentMap(entry, environmentmap ? environmentmap->GetTexture() : texturehandle_t {});
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

}
