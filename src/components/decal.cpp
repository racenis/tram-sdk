// Tramway Drifting and Dungeon Exploration Simulator SDK Runtime

#include <components/decal.h>

#include <components/render.h>

#include <render/api.h>
#include <render/scene.h>
#include <render/vertices.h>
#include <render/lightmap.h>
#include <render/material.h>

#include <framework/logging.h>

#include <config.h>

/**
 * @class tram::DecalComponent components/decal.h <components/decal.h>
 * 
 * Projects decals onto the scene.
 * @see https://racenis.github.io/tram-sdk/documentation/components/decal.html
 */

/*
 * FUTURE IMPROVEMENTS
 * 
 * Currently the decal uses flat normals, so it looks weird on curved surfaces.
 * The problem is that Models still don't store the actual normals for vertices.
 * 
 * We should also add in proper lightmap UVs and projections to posed models.
 * 
 * But for this we need to modify our model storage format.
 * 
 * */

namespace tram {
using namespace tram::Render;

template <> Pool<DecalComponent> PoolProxy<DecalComponent>::pool("DecalComponent pool", COMPONENT_LIMIT_DECAL);
template <> void Component<DecalComponent>::init() { ptr = DecalComponent::Make(); }
template <> void Component<DecalComponent>::yeet() { DecalComponent::Yeet(ptr); }

using namespace API;

struct DecalVertices {
    std::vector<Render::StaticModelVertex> static_verts;
    std::vector<Render::StaticModelVertex> dynamic_verts;
};

void DecalComponent::InitAsStatic() {
    if (is_static) return;
    
    is_static = true;
    vertices = new DecalVertices;
    
    CreateVertexArray(GetVertexDefinition(VERTEX_STATIC), vertex_array);

    auto texture_handle = sprite->GetMaterial()->GetMaterial();

    draw_list_entry = InsertDrawListEntry();
    SetDrawListVertexArray(draw_list_entry, vertex_array);
    SetDrawListIndexRange(draw_list_entry, 0, 0);
    SetFlags(draw_list_entry, FLAG_RENDER | FLAG_DECAL);
    SetLayer(draw_list_entry, LAYER_DEFAULT);
    SetDrawListMaterials(draw_list_entry, 1, &texture_handle);
    SetLightmap(draw_list_entry, Lightmap::Find("fullbright")->GetTexture());
    SetDrawListShader(draw_list_entry, VERTEX_STATIC, MATERIAL_TEXTURE_ALPHA);
}

void DecalComponent::InitAsDynamic() {
    if (is_dynamic) return;
    
    is_dynamic = true;
    vertices = new DecalVertices;
    
    CreateVertexArray(GetVertexDefinition(VERTEX_DYNAMIC), vertex_array);

    auto texture_handle = sprite->GetMaterial()->GetMaterial();

    draw_list_entry = InsertDrawListEntry();
    SetDrawListVertexArray(draw_list_entry, vertex_array);
    SetDrawListIndexRange(draw_list_entry, 0, 0);
    SetFlags(draw_list_entry, FLAG_RENDER | FLAG_DECAL);
    SetLayer(draw_list_entry, LAYER_DEFAULT);
    SetDrawListMaterials(draw_list_entry, 1, &texture_handle);
    SetEnvironmentMap(draw_list_entry, Environment::Find("fulldark")->GetTexture());
    SetDrawListShader(draw_list_entry, VERTEX_DYNAMIC, MATERIAL_TEXTURE_ALPHA);
}

DecalComponent::~DecalComponent() {
    is_ready = false;
    
    if (vertices) delete vertices;
    vertices = nullptr;
    
    Render::API::RemoveDrawListEntry(draw_list_entry);
};

void DecalComponent::Start() {
    if (!sprite) {
        Log(Severity::CRITICAL_ERROR, System::RENDER, "Decal component doesn't have its sprite set!");
    }
    
    InitAsStatic();
    
    is_ready = true;
    UpdateRenderListObject();
}

void DecalComponent::Update() {
    if (!is_ready) return;
}

// note that aabb boxes and clipping planes are calculated in world space.
// this is a bit ineffiecient, but whatevs.
struct DecalProjectInfo {
    vec3 aabb_min;
    vec3 aabb_max;
    float decal_width;
    float decal_height;
    float tex_width;
    float tex_height;
    float tex_w_off;
    float tex_h_off;
    
    vec4 front_plane;
    vec4 back_plane;
    vec4 top_plane;
    vec4 bottom_plane;
    vec4 left_plane;
    vec4 right_plane;
};

void DecalComponent::ResetVertices() {
    if (vertices && is_dynamic) vertices->dynamic_verts.clear();
    if (vertices && is_static) vertices->static_verts.clear();
}

void DecalComponent::ProjectOnWorld(bool reset) {
    ProjectOnWorld(location, rotation, reset);
}

void DecalComponent::ProjectOnWorld(vec3 pos, quat rot, bool reset) {
    if (!is_ready) return;
    if (reset) ResetVertices();

    DecalProjectInfo info;
    MakeProjectInfo(info, pos, rot);
    
    // decal info is already in worldspace, so we can just pass everything in as is
    AABB::FindAllIntersectionsFromAABB(info.aabb_min, info.aabb_max, [&](AABB::ReferenceType type, EntityComponent* component) {
        if (type != Render::AABB::REFERENCE_RENDERCOMPONENT) return;
        RenderComponent* rcomp = (RenderComponent*)component;
        
        // TODO: skip posed rendercomps
        
        ProjectOnModel(info, rcomp, nullptr, pos, rot);
    });
}

void DecalComponent::ProjectOnModel(RenderComponent* comp, vec3 pos, quat rot, bool reset) {
    ProjectOnModel(comp, nullptr, pos, rot, reset);
}

void DecalComponent::ProjectOnModel(RenderComponent* comp, AnimationComponent* anim, vec3 pos, quat rot, bool reset) {
    if (!is_ready) return;
    if (reset) ResetVertices();
    
    if (anim && is_static) assert(false);
    if (!anim && is_dynamic) assert(false);
    
    DecalProjectInfo info;
    MakeProjectInfo(info, pos, rot);
    
    ProjectOnModel(info, comp, anim, pos, rot);
}

static vec4 GetPlaneEquation(vec3 a, vec3 b, vec3 c) {
    vec3 normal = glm::cross(b - a, c - a);
    return vec4(normal, -glm::dot(a, normal));
}

static float PlaneIntersection(vec4 plane, vec3 a, vec3 b) {
    float denom = plane.x * (b.x - a.x) + plane.y * (b.y - a.y) + plane.z * (b.z - a.z);
    if (denom == 0.0f) {
        return INFINITY;
    }
    
    float t = -(plane.x * a.x + plane.y * a.y + plane.z * a.z + plane.w) / denom;
    return t;
}

static void ClipAABBTriangle(AABBTriangle triangle, vec4 plane, auto callback) {
    bool keep_p1 = glm::dot(vec4(triangle.point1, 1.0f), plane) < 0.0f;
    bool keep_p2 = glm::dot(vec4(triangle.point2, 1.0f), plane) < 0.0f;
    bool keep_p3 = glm::dot(vec4(triangle.point3, 1.0f), plane) < 0.0f;
    
    if (keep_p1 && keep_p2 && keep_p3) {
        callback(triangle);
        return;
    }
    
    if (!keep_p1 && !keep_p2 && !keep_p3) {
         return;
    }

    if (keep_p1 && keep_p2) {
        float a = PlaneIntersection(plane, triangle.point1, triangle.point3);
        float b = PlaneIntersection(plane, triangle.point2, triangle.point3);
        
        AABBTriangle triangle_a = triangle;
        AABBTriangle triangle_b = triangle;
        
        triangle_a.point3 = glm::mix(triangle.point1, triangle.point3, a);
        triangle_b.point3 = glm::mix(triangle.point2, triangle.point3, b);
        triangle_b.point1 = triangle_a.point3;
        
        callback(triangle_a);
        callback(triangle_b);
        
        return;
    }
    
    if (keep_p1 && keep_p3) {
        float a = PlaneIntersection(plane, triangle.point1, triangle.point2);
        float b = PlaneIntersection(plane, triangle.point3, triangle.point2);
        
        AABBTriangle triangle_a = triangle;
        AABBTriangle triangle_b = triangle;
        
        triangle_a.point2 = glm::mix(triangle.point1, triangle.point2, a);
        triangle_b.point2 = glm::mix(triangle.point3, triangle.point2, b);
        triangle_b.point1 = triangle_a.point2;
        
        callback(triangle_a);
        callback(triangle_b);
        
        return;
    }
    
    if (keep_p2 && keep_p3) {
        float a = PlaneIntersection(plane, triangle.point2, triangle.point1);
        float b = PlaneIntersection(plane, triangle.point3, triangle.point1);
        
        AABBTriangle triangle_a = triangle;
        AABBTriangle triangle_b = triangle;
        
        triangle_a.point1 = glm::mix(triangle.point2, triangle.point1, a);
        triangle_b.point1 = glm::mix(triangle.point3, triangle.point1, b);
        triangle_b.point2 = triangle_a.point1;
        
        callback(triangle_a);
        callback(triangle_b);
        
        return;
    }
    
    
    if (keep_p1) {
        float a = PlaneIntersection(plane, triangle.point2, triangle.point1);
        float b = PlaneIntersection(plane, triangle.point3, triangle.point1);
        
        triangle.point2 = glm::mix(triangle.point2, triangle.point1, a);
        triangle.point3 = glm::mix(triangle.point3, triangle.point1, b);
        
        callback(triangle);
        
        return;
    }
    
    if (keep_p2) {
        float a = PlaneIntersection(plane, triangle.point1, triangle.point2);
        float b = PlaneIntersection(plane, triangle.point3, triangle.point2);
        
        triangle.point1 = glm::mix(triangle.point1, triangle.point2, a);
        triangle.point3 = glm::mix(triangle.point3, triangle.point2, b);
        
        callback(triangle);
        
        return;
    }
    
    if (keep_p3) {
        float a = PlaneIntersection(plane, triangle.point1, triangle.point3);
        float b = PlaneIntersection(plane, triangle.point2, triangle.point3);
        
        triangle.point1 = glm::mix(triangle.point1, triangle.point3, a);
        triangle.point2 = glm::mix(triangle.point2, triangle.point3, b);
        
        callback(triangle);
        
        return;
    }
}

void DecalComponent::MakeProjectInfo(DecalProjectInfo& info, vec3 pos, quat rot) {
    const float frame_width = sprite->GetFrames()[frame].width;
    const float frame_height = sprite->GetFrames()[frame].height;

    info.tex_width = frame_width / (float)sprite->GetMaterial()->GetWidth();
    info.tex_height = frame_height / (float)sprite->GetMaterial()->GetHeight();
    info.tex_w_off = (float)sprite->GetFrames()[frame].offset_x / (float)sprite->GetMaterial()->GetWidth();
    info.tex_h_off = (float)sprite->GetFrames()[frame].offset_y / (float)sprite->GetMaterial()->GetHeight();
    info.tex_h_off = 1.0f - info.tex_h_off - info.tex_height;

    info.decal_width = frame_width / 32.0f * scale;
    info.decal_height = frame_height / 32.0f * scale;

    const float half_width = info.decal_width / 2.0f;
    const float half_height = info.decal_height / 2.0f;
    
    vec3 normal = rot * DIRECTION_FORWARD;

    vec3 top_left_front = {-half_width, half_height, 0.0f};
    vec3 top_right_front = {half_width, half_height, 0.0f};
    vec3 bottom_left_front = {-half_width, -half_height, 0.0f};
    vec3 bottom_right_front = {half_width, -half_height, 0.0f};
    
    top_left_front = pos + rot * top_left_front;
    top_right_front = pos + rot * top_right_front;
    bottom_left_front = pos + rot * bottom_left_front;
    bottom_right_front = pos + rot * bottom_right_front;
    
    vec3 top_left_back = top_left_front + normal * depth;
    vec3 top_right_back = top_right_front + normal * depth;
    vec3 bottom_left_back = bottom_left_front + normal * depth;
    vec3 bottom_right_back = bottom_right_front + normal * depth;

    info.aabb_min = top_left_front;
    info.aabb_max = top_left_front;
    
    info.aabb_min = MergeAABBMin(info.aabb_min, top_right_front);
    info.aabb_min = MergeAABBMin(info.aabb_min, bottom_left_front);
    info.aabb_min = MergeAABBMin(info.aabb_min, bottom_right_front);
    
    info.aabb_min = MergeAABBMin(info.aabb_min, top_left_back);
    info.aabb_min = MergeAABBMin(info.aabb_min, top_right_back);
    info.aabb_min = MergeAABBMin(info.aabb_min, bottom_left_back);
    info.aabb_min = MergeAABBMin(info.aabb_min, bottom_right_back);
    
    info.aabb_max = MergeAABBMax(info.aabb_max, top_right_front);
    info.aabb_max = MergeAABBMax(info.aabb_max, bottom_left_front);
    info.aabb_max = MergeAABBMax(info.aabb_max, bottom_right_front);
    
    info.aabb_max = MergeAABBMax(info.aabb_max, top_left_back);
    info.aabb_max = MergeAABBMax(info.aabb_max, top_right_back);
    info.aabb_max = MergeAABBMax(info.aabb_max, bottom_left_back);
    info.aabb_max = MergeAABBMax(info.aabb_max, bottom_right_back);

    info.top_plane = GetPlaneEquation(top_left_front, top_right_front, top_left_back);
    info.bottom_plane = GetPlaneEquation(bottom_left_front, bottom_left_back, bottom_right_front);
    
    info.left_plane = GetPlaneEquation(top_left_front, bottom_left_back, bottom_left_front);
    info.right_plane = GetPlaneEquation(top_right_front, bottom_right_front, bottom_right_back);
 
    info.front_plane = GetPlaneEquation(top_left_front, bottom_left_front, bottom_right_front);
    info.back_plane = GetPlaneEquation(top_left_back, bottom_right_back, bottom_left_back);
}

void DecalComponent::ProjectOnModel(DecalProjectInfo& info, RenderComponent* comp, AnimationComponent* anim, vec3 pos, quat rot) {
    if (anim) InitAsDynamic();
    if (!anim) InitAsStatic();
    
    mat4 worldspace_to_decalspace = glm::inverse(PositionRotationToMatrix(location, rotation));
    
    vec3 local_min = (info.aabb_min - comp->GetLocation()) / comp->GetScale();
    vec3 local_max = (info.aabb_max - comp->GetLocation()) / comp->GetScale();
    
    RotateAABB(local_min, local_max, -comp->GetRotation());
    
    std::vector<AABBTriangle> tris;
    comp->GetModel()->FindAllFromAABB(local_min, local_max, tris);
    
    for (auto& tri : tris) {
        tri.point1 = comp->GetLocation() + comp->GetRotation() * (comp->GetScale() * tri.point1);
        tri.point2 = comp->GetLocation() + comp->GetRotation() * (comp->GetScale() * tri.point2);
        tri.point3 = comp->GetLocation() + comp->GetRotation() * (comp->GetScale() * tri.point3);
        
        tri.normal = tri.normal * comp->GetRotation();
        
        ClipAABBTriangle(tri, info.front_plane, [&](AABBTriangle tri) {
        ClipAABBTriangle(tri, info.back_plane, [&](AABBTriangle tri) {
        ClipAABBTriangle(tri, info.top_plane, [&](AABBTriangle tri) {
        ClipAABBTriangle(tri, info.bottom_plane, [&](AABBTriangle tri) {
        ClipAABBTriangle(tri, info.left_plane, [&](AABBTriangle tri) {
        ClipAABBTriangle(tri, info.right_plane, [&](AABBTriangle tri) {
            
            Render::StaticModelVertex vert1;
            Render::StaticModelVertex vert2;
            Render::StaticModelVertex vert3;
            
            // position of vertices in projecion space
            vec3 local1 = glm::inverse(rot) * (tri.point1 - pos);
            vec3 local2 = glm::inverse(rot) * (tri.point2 - pos);
            vec3 local3 = glm::inverse(rot) * (tri.point3 - pos);
            
            vert1.co = worldspace_to_decalspace * vec4(tri.point1, 1.0f);
            vert2.co = worldspace_to_decalspace * vec4(tri.point2, 1.0f);
            vert3.co = worldspace_to_decalspace * vec4(tri.point3, 1.0f);
            
            vert1.normal = glm::normalize(worldspace_to_decalspace * vec4(tri.normal, 0.0f));
            vert2.normal = glm::normalize(worldspace_to_decalspace * vec4(tri.normal, 0.0f));
            vert3.normal = glm::normalize(worldspace_to_decalspace * vec4(tri.normal, 0.0f));
            
            vert1.tex = {local1.x / info.decal_width + 0.5f, local1.y / info.decal_height + 0.5f};
            vert2.tex = {local2.x / info.decal_width + 0.5f, local2.y / info.decal_height + 0.5f};
            vert3.tex = {local3.x / info.decal_width + 0.5f, local3.y / info.decal_height + 0.5f};
            
            vert1.tex.x *= info.tex_width;
            vert2.tex.x *= info.tex_width;
            vert3.tex.x *= info.tex_width;
            
            vert1.tex.y *= info.tex_height;
            vert2.tex.y *= info.tex_height;
            vert3.tex.y *= info.tex_height;
            
            vert1.tex.x += info.tex_w_off;
            vert2.tex.x += info.tex_w_off;
            vert3.tex.x += info.tex_w_off;
            
            vert1.tex.y += info.tex_h_off + 1.0f;
            vert2.tex.y += info.tex_h_off + 1.0f;
            vert3.tex.y += info.tex_h_off + 1.0f;
            
            // TODO: implement lightmap calculation
            vert1.lighttex = {0.0f, 0.0f};
            vert2.lighttex = {0.0f, 1.0f};
            vert3.lighttex = {1.0f, 1.0f};
            
            vert1.texture = 0;
            vert2.texture = 0;
            vert3.texture = 0;
            
            vertices->static_verts.push_back(vert1);
            vertices->static_verts.push_back(vert2);
            vertices->static_verts.push_back(vert3);
        });
        });
        });
        });
        });
        });
    }
    
    if (is_static) {
        Render::API::UpdateVertexArray(vertex_array, sizeof(Render::StaticModelVertex) * vertices->static_verts.size(), vertices->static_verts.data());
        Render::API::SetDrawListIndexRange(draw_list_entry, 0, vertices->static_verts.size());
    }
    
    if (is_dynamic) {
        Render::API::UpdateVertexArray(vertex_array, sizeof(Render::DynamicModelVertex) * vertices->dynamic_verts.size(), vertices->dynamic_verts.data());
        Render::API::SetDrawListIndexRange(draw_list_entry, 0, vertices->dynamic_verts.size());
    }
}

void DecalComponent::UpdateRenderListObject() {
    if (!is_ready) return;
    
    Render::API::SetMatrix(draw_list_entry, PositionRotationToMatrix(location, rotation));
}

/// Creates a new DecalComponent.
DecalComponent* DecalComponent::Make() {
    DecalComponent* ptr = PoolProxy<DecalComponent>::GetPool().allocate();
    new(ptr) DecalComponent();
    return ptr;
}

/// Deletes an DecalComponent.
void DecalComponent::Yeet(DecalComponent* component) {
    component->~DecalComponent();
    PoolProxy<DecalComponent>::GetPool().deallocate(component);
}

}