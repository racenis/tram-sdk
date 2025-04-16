// Tramway Drifting and Dungeon Exploration Simulator SDK Runtime

#include <render/raytrace/buffer.h>

namespace tram::Render::API {

static Pool<RTVertexArray> vertex_arrays("vertex_arrays list", 200);
static Pool<RTIndexArray> index_arrays("index_arrays list", 200);

void ParseFormat(vertexarray_t& vertex_array, VertexDefinition vertex_format) {
    auto& helper = vertex_array.rt_vertex_array->helper;
 
     for (int i = 0; i < (int)vertex_format.attribute_count; i++) {
        switch (vertex_format.attributes[i].ffp_type) {
            case VertexAttribute::FFP_POSITION:     helper.position = i;    break;
            case VertexAttribute::FFP_NORMAL:       helper.normal = i;      break;
            case VertexAttribute::FFP_COLOR:        helper.color = i;      break;
            case VertexAttribute::FFP_TEXTURE:      helper.texture = i;     break;
            case VertexAttribute::FFP_LIGHTMAP:     helper.lightmap = i;    break;
            case VertexAttribute::FFP_BONE_INDEX:   helper.bone_index = i;  break;
            case VertexAttribute::FFP_BONE_WEIGHT:  helper.bone_weight = i; break;
            case VertexAttribute::FFP_IGNORE:                               break;
        }
    }
    
    if (helper.color != -1) {
         vertex_array.rt_vertex_array->type = RT_STATIC_COLORED;
    }
 
    if (helper.lightmap != -1) {
        vertex_array.rt_vertex_array->type = RT_STATIC_LIGHTMAPPED;
    }
    
    if (helper.bone_index != -1 && helper.bone_weight != -1) {
        vertex_array.rt_vertex_array->type = RT_DYNAMIC_BLENDED;
    }
    
    vertex_array.rt_vertex_array->format = vertex_format;
}

void PackVertices(vertexarray_t& vertex_array, void* data, size_t count) {
    switch (vertex_array.rt_vertex_array->type) {
        case RT_STATIC_LIGHTMAPPED: {
            StaticVertex* verts = (StaticVertex*)malloc(sizeof(StaticVertex) * count);
            
            VertexAttribute pos_attrib = vertex_array.rt_vertex_array->format.attributes[vertex_array.rt_vertex_array->helper.position];
            for (size_t i = 0; i < count; i++) {
                vec3* pos = (vec3*)((char*)data + pos_attrib.offset + pos_attrib.stride * i);
                verts[i].pos = *pos;
            }
            
            VertexAttribute nrm_attrib = vertex_array.rt_vertex_array->format.attributes[vertex_array.rt_vertex_array->helper.normal];
            for (size_t i = 0; i < count; i++) {
                vec3* nrm = (vec3*)((char*)data + nrm_attrib.offset + nrm_attrib.stride * i);
                verts[i].nrm = *nrm;
            }
            
            VertexAttribute tex_attrib = vertex_array.rt_vertex_array->format.attributes[vertex_array.rt_vertex_array->helper.texture];
            for (size_t i = 0; i < count; i++) {
                vec2* tex = (vec2*)((char*)data + tex_attrib.offset + tex_attrib.stride * i);
                verts[i].tex = *tex;
            }
            
            VertexAttribute lit_attrib = vertex_array.rt_vertex_array->format.attributes[vertex_array.rt_vertex_array->helper.lightmap];
            for (size_t i = 0; i < count; i++) {
                vec2* lit = (vec2*)((char*)data + lit_attrib.offset + lit_attrib.stride * i);
                verts[i].lit = *lit;
            }
            
            vertex_array.rt_vertex_array->vertices = verts;
            vertex_array.rt_vertex_array->vertex_count = count;
        } break;
        case RT_DYNAMIC_BLENDED: {
            DynamicVertex* verts = (DynamicVertex*)malloc(sizeof(DynamicVertex) * count);
            
            VertexAttribute pos_attrib = vertex_array.rt_vertex_array->format.attributes[vertex_array.rt_vertex_array->helper.position];
            for (size_t i = 0; i < count; i++) {
                vec3* pos = (vec3*)((char*)data + pos_attrib.offset + pos_attrib.stride * i);
                verts[i].pos = *pos;
            }
            
            VertexAttribute nrm_attrib = vertex_array.rt_vertex_array->format.attributes[vertex_array.rt_vertex_array->helper.normal];
            for (size_t i = 0; i < count; i++) {
                vec3* nrm = (vec3*)((char*)data + nrm_attrib.offset + nrm_attrib.stride * i);
                verts[i].nrm = *nrm;
            }
            
            VertexAttribute tex_attrib = vertex_array.rt_vertex_array->format.attributes[vertex_array.rt_vertex_array->helper.texture];
            for (size_t i = 0; i < count; i++) {
                vec2* tex = (vec2*)((char*)data + tex_attrib.offset + tex_attrib.stride * i);
                verts[i].tex = *tex;
            }
            
            VertexAttribute wgt_attrib = vertex_array.rt_vertex_array->format.attributes[vertex_array.rt_vertex_array->helper.bone_weight];
            for (size_t i = 0; i < count; i++) {
                vec4* wgt = (vec4*)((char*)data + wgt_attrib.offset + wgt_attrib.stride * i);
                verts[i].wgt = glm::normalize(*wgt);
            }
            
            VertexAttribute ind_attrib = vertex_array.rt_vertex_array->format.attributes[vertex_array.rt_vertex_array->helper.bone_index];
            for (size_t i = 0; i < count; i++) {
                ivec4* ind = (ivec4*)((char*)data + ind_attrib.offset + ind_attrib.stride * i);
                verts[i].ind = *ind;
            }
        
            vertex_array.rt_vertex_array->vertices = verts;
            vertex_array.rt_vertex_array->vertex_count = count;
        } break;
        case RT_STATIC_COLORED: {
            ColorVertex* verts = (ColorVertex*)malloc(sizeof(ColorVertex) * count);
            
            VertexAttribute pos_attrib = vertex_array.rt_vertex_array->format.attributes[vertex_array.rt_vertex_array->helper.position];
            for (size_t i = 0; i < count; i++) {
                vec3* pos = (vec3*)((char*)data + pos_attrib.offset + pos_attrib.stride * i);
                verts[i].pos = *pos;
            }
            
            VertexAttribute col_attrib = vertex_array.rt_vertex_array->format.attributes[vertex_array.rt_vertex_array->helper.color];
            for (size_t i = 0; i < count; i++) {
                vec3* col = (vec3*)((char*)data + col_attrib.offset + col_attrib.stride * i);
                verts[i].col = *col;
            }
            
            vertex_array.rt_vertex_array->vertices = verts;
            vertex_array.rt_vertex_array->vertex_count = count;
        } break;
        case RT_SPRITE:
            assert(false);
    }
}

void CreateIndexedVertexArray(VertexDefinition vertex_format, vertexarray_t& vertex_array, indexarray_t& index_array, size_t vertex_size, void* vertex_data, size_t index_size, void* index_data) {
    vertex_array.rt_vertex_array = vertex_arrays.AddNew();
    ParseFormat(vertex_array, vertex_format);
    PackVertices(vertex_array, vertex_data, vertex_size / vertex_format.attributes[0].stride);
    
    index_array.rt_index_array = index_arrays.AddNew();
    index_array.rt_index_array->indices = (uint32_t*)malloc(index_size);
    index_array.rt_index_array->index_count = index_size / sizeof(uint32_t);
    memcpy(index_array.rt_index_array->indices, index_data, index_size);
}

void CreateVertexArray(VertexDefinition vertex_format, vertexarray_t& vertex_array) {
    vertex_array.rt_vertex_array = vertex_arrays.AddNew();
    ParseFormat(vertex_array, vertex_format);
}

void UpdateVertexArray(vertexarray_t& vertex_array, size_t data_size, void* data) {
    if (vertex_array.rt_vertex_array->vertices) {
        free(vertex_array.rt_vertex_array->vertices);
        vertex_array.rt_vertex_array->vertices = nullptr;
    }
    
    PackVertices(vertex_array, data, data_size / vertex_array.rt_vertex_array->format.attributes[0].stride);
}

}