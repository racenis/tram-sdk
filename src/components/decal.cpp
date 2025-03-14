// Tramway Drifting and Dungeon Exploration Simulator SDK Runtime

#include <components/decal.h>

#include <components/render.h>

#include <render/api.h>
#include <render/scene.h>
#include <render/vertices.h>
#include <render/material.h>

#include <framework/logging.h>

#include <config.h>

/**
 * @class tram::DecalComponent components/decal.h <components/decal.h>
 * 
 * Projects decals onto the scene.
 * @see https://racenis.github.io/tram-sdk/documentation/components/decal.html
 */

namespace tram {
using namespace tram::Render;

template <> Pool<DecalComponent> PoolProxy<DecalComponent>::pool("DecalComponent pool", COMPONENT_LIMIT_DECAL);
template <> void Component<DecalComponent>::init() { ptr = PoolProxy<DecalComponent>::New(); }
template <> void Component<DecalComponent>::yeet() { PoolProxy<DecalComponent>::Delete(ptr); }

using namespace API;

DecalComponent::~DecalComponent() {
    is_ready = false;

    Render::API::RemoveDrawListEntry(draw_list_entry);
};

void DecalComponent::Start() {
    assert(!is_ready);

    CreateVertexArray(GetVertexDefinition(VERTEX_STATIC), vertex_array);

    auto texture_handle = sprite->GetMaterial()->GetMaterial();

    draw_list_entry = InsertDrawListEntry();
    SetDrawListVertexArray(draw_list_entry, vertex_array);
    SetDrawListIndexRange(draw_list_entry, 0, 0);
    SetFlags(draw_list_entry, FLAG_RENDER);
    SetDrawListMaterials(draw_list_entry, 1, &texture_handle);
    SetLightmap(draw_list_entry, Material::Find("fullbright")->GetTexture());
    SetDrawListShader(draw_list_entry, VERTEX_STATIC, MATERIAL_TEXTURE_ALPHA);

    is_ready = true;
    UpdateRenderListObject();
}

void DecalComponent::Update() {
    if (!is_ready) return;
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

void DecalComponent::UpdateRenderListObject() {
    if (!is_ready) return;

    const float frame_width = sprite->GetFrames()[frame].width;
    const float frame_height = sprite->GetFrames()[frame].height;

    float tex_width = frame_width / (float)sprite->GetMaterial()->GetWidth();
    float tex_height = frame_height / (float)sprite->GetMaterial()->GetHeight();
    float tex_w_off = (float)sprite->GetFrames()[frame].offset_x / (float)sprite->GetMaterial()->GetWidth();
    float tex_h_off = (float)sprite->GetFrames()[frame].offset_y / (float)sprite->GetMaterial()->GetHeight();
    tex_h_off = 1.0f - tex_h_off - tex_height;

    const float decal_width = frame_width / 32.0f * scale;
    const float decal_height = frame_height / 32.0f * scale;

    const float half_width = decal_width / 2.0f;
    const float half_height = decal_height / 2.0f;

    (void)tex_width;
    (void)tex_height;
    (void)tex_w_off;
    (void)tex_h_off;

    vec3 normal = rotation * DIRECTION_FORWARD;

    vec3 top_left_front = {-half_width, half_height, 0.0f};
    vec3 top_right_front = {half_width, half_height, 0.0f};
    vec3 bottom_left_front = {-half_width, -half_height, 0.0f};
    vec3 bottom_right_front = {half_width, -half_height, 0.0f};
    
    top_left_front = location + rotation * top_left_front;
    top_right_front = location + rotation * top_right_front;
    bottom_left_front = location + rotation * bottom_left_front;
    bottom_right_front = location + rotation * bottom_right_front;
    
    vec3 top_left_back = top_left_front + normal;
    vec3 top_right_back = top_right_front + normal;
    vec3 bottom_left_back = bottom_left_front + normal;
    vec3 bottom_right_back = bottom_right_front + normal;

    vec3 aabb_min = top_left_front;
    vec3 aabb_max = top_left_front;
    
    aabb_min = MergeAABBMin(aabb_min, top_right_front);
    aabb_min = MergeAABBMin(aabb_min, bottom_left_front);
    aabb_min = MergeAABBMin(aabb_min, bottom_right_front);
    
    aabb_min = MergeAABBMin(aabb_min, top_left_back);
    aabb_min = MergeAABBMin(aabb_min, top_right_back);
    aabb_min = MergeAABBMin(aabb_min, bottom_left_back);
    aabb_min = MergeAABBMin(aabb_min, bottom_right_back);
    
    aabb_max = MergeAABBMax(aabb_max, top_right_front);
    aabb_max = MergeAABBMax(aabb_max, bottom_left_front);
    aabb_max = MergeAABBMax(aabb_max, bottom_right_front);
    
    aabb_max = MergeAABBMax(aabb_max, top_left_back);
    aabb_max = MergeAABBMax(aabb_max, top_right_back);
    aabb_max = MergeAABBMax(aabb_max, bottom_left_back);
    aabb_max = MergeAABBMax(aabb_max, bottom_right_back);


    vec4 top_plane = GetPlaneEquation(top_left_front, top_right_front, top_left_back);
    vec4 bottom_plane = GetPlaneEquation(bottom_left_front, bottom_left_back, bottom_right_front);
    
    vec4 left_plane = GetPlaneEquation(top_left_front, bottom_left_back, bottom_left_front);
    vec4 right_plane = GetPlaneEquation(top_right_front, bottom_right_front, bottom_right_back);
 
    vec4 front_plane = GetPlaneEquation(top_left_front, bottom_left_front, bottom_right_front);
    vec4 back_plane = GetPlaneEquation(top_left_back, bottom_right_back, bottom_left_back);

    std::vector<Render::StaticModelVertex> verts;

    AABB::FindAllIntersectionsFromAABB(aabb_min, aabb_max, [&](AABB::ReferenceType type, EntityComponent* component) {
        if (type != Render::AABB::REFERENCE_RENDERCOMPONENT) return;
        RenderComponent* rcomp = (RenderComponent*)component;
        
        vec3 local_min = aabb_min - rcomp->GetLocation();
        vec3 local_max = aabb_max - rcomp->GetLocation();
        
        RotateAABB(local_min, local_max, -rcomp->GetRotation());
        
        std::vector<AABBTriangle> tris;
        
        rcomp->GetModel()->FindAllFromAABB(local_min, local_max, tris);
        
        for (auto& tri : tris) {
            tri.point1 = rcomp->GetLocation() + rcomp->GetRotation() * tri.point1;
            tri.point2 = rcomp->GetLocation() + rcomp->GetRotation() * tri.point2;
            tri.point3 = rcomp->GetLocation() + rcomp->GetRotation() * tri.point3;
            
            tri.normal = tri.normal * rcomp->GetRotation();
            
            ClipAABBTriangle(tri, front_plane, [&](AABBTriangle tri) {
            ClipAABBTriangle(tri, back_plane, [&](AABBTriangle tri) {
            ClipAABBTriangle(tri, top_plane, [&](AABBTriangle tri) {
            ClipAABBTriangle(tri, bottom_plane, [&](AABBTriangle tri) {
            ClipAABBTriangle(tri, left_plane, [&](AABBTriangle tri) {
            ClipAABBTriangle(tri, right_plane, [&](AABBTriangle tri) {
                
                Render::StaticModelVertex vert1;
                Render::StaticModelVertex vert2;
                Render::StaticModelVertex vert3;
                
                vec3 local1 = glm::inverse(rotation) * (tri.point1 - location);
                vec3 local2 = glm::inverse(rotation) * (tri.point2 - location);
                vec3 local3 = glm::inverse(rotation) * (tri.point3 - location);
                
                vert1.co = tri.point1 + tri.normal * 0.01f;
                vert2.co = tri.point2 + tri.normal * 0.01f;
                vert3.co = tri.point3 + tri.normal * 0.01f;
                
                vert1.normal = tri.normal;
                vert2.normal = tri.normal;
                vert3.normal = tri.normal;
                
                vert1.tex = {local1.x / decal_width + 0.5f, local1.y / decal_height + 0.5f};
                vert2.tex = {local2.x / decal_width + 0.5f, local2.y / decal_height + 0.5f};
                vert3.tex = {local3.x / decal_width + 0.5f, local3.y / decal_height + 0.5f};
                
                vert1.tex.x *= tex_width;
                vert2.tex.x *= tex_width;
                vert3.tex.x *= tex_width;
                
                vert1.tex.y *= tex_height;
                vert2.tex.y *= tex_height;
                vert3.tex.y *= tex_height;
                
                vert1.tex.x += tex_w_off;
                vert2.tex.x += tex_w_off;
                vert3.tex.x += tex_w_off;
                
                vert1.tex.y += tex_h_off + 1.0f;
                vert2.tex.y += tex_h_off + 1.0f;
                vert3.tex.y += tex_h_off + 1.0f;
                
                vert1.lighttex = {0.0f, 0.0f};
                vert2.lighttex = {0.0f, 1.0f};
                vert3.lighttex = {1.0f, 1.0f};
                
                vert1.texture = 0;
                vert2.texture = 0;
                vert3.texture = 0;
                
                verts.push_back(vert1);
                verts.push_back(vert2);
                verts.push_back(vert3);
            });
            });
            });
            });
            });
            });
        }

    });

    Render::API::UpdateVertexArray(vertex_array, sizeof(Render::StaticModelVertex) * verts.size(), verts.data());
    Render::API::SetDrawListIndexRange(draw_list_entry, 0, verts.size());
    
    Render::API::SetMatrix(draw_list_entry, PositionRotationToMatrix(vec3(0.0f, 0.0f, 0.0f), quat(1.0f, 0.0f, 0.0f, 0.0f)));
}

}