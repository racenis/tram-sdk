// Tramway Drifting and Dungeon Exploration Simulator SDK Runtime

#include <render/opengl/sprite.h>

#include <config.h>

#ifndef _WIN32
    #include <GL/gl.h>
    #include <GLES3/gl3.h>
#else
    #include <glad.h>
#endif

using namespace tram;

namespace tram::Render::API {

void CreateIndexedVertexArray(VertexDefinition vertex_format, vertexarray_t& vertex_array, indexarray_t& index_array, size_t vertex_size, void* vertex_data, size_t index_size, void* index_data) {
    glGenBuffers(1, &vertex_array.gl_vertex_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertex_array.gl_vertex_buffer);
    glBufferData(GL_ARRAY_BUFFER, vertex_size, vertex_data, GL_STATIC_DRAW);

    glGenBuffers(1, &index_array.gl_index_buffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_array.gl_index_buffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, index_size, index_data, GL_STATIC_DRAW);

    glGenVertexArrays(1, &vertex_array.gl_vertex_array);

    glBindVertexArray(vertex_array.gl_vertex_array);

    for (size_t i = 0; i < vertex_format.attribute_count; i++) {
        uint32_t opengl_type = vertex_format.attributes[i].type == VertexAttribute::FLOAT32 ? GL_FLOAT : GL_UNSIGNED_INT;
        
        if (opengl_type == GL_FLOAT) {
            glVertexAttribPointer(i, vertex_format.attributes[i].size, opengl_type, GL_FALSE, vertex_format.attributes[i].stride, (void*)vertex_format.attributes[i].offset);
        } else {
            glVertexAttribIPointer(i, vertex_format.attributes[i].size, opengl_type, vertex_format.attributes[i].stride, (void*)vertex_format.attributes[i].offset);
        }
        
        glEnableVertexAttribArray(i);
    }

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_array.gl_index_buffer);
    
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void CreateVertexArray(VertexDefinition vertex_format, vertexarray_t& vertex_array) {
    glGenBuffers(1, &vertex_array.gl_vertex_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertex_array.gl_vertex_buffer);
    glBufferData(GL_ARRAY_BUFFER, 0, nullptr, GL_DYNAMIC_DRAW);

    glGenVertexArrays(1, &vertex_array.gl_vertex_array);

    glBindVertexArray(vertex_array.gl_vertex_array);

    for (size_t i = 0; i < vertex_format.attribute_count; i++) {
        uint32_t opengl_type = vertex_format.attributes[i].type == VertexAttribute::FLOAT32 ? GL_FLOAT : GL_UNSIGNED_INT;
        
        if (opengl_type == GL_FLOAT) {
            glVertexAttribPointer(i, vertex_format.attributes[i].size, opengl_type, GL_FALSE, vertex_format.attributes[i].stride, (void*)vertex_format.attributes[i].offset);
        } else {
            glVertexAttribIPointer(i, vertex_format.attributes[i].size, opengl_type, vertex_format.attributes[i].stride, (void*)vertex_format.attributes[i].offset);
        }
        
        glEnableVertexAttribArray(i);
    }

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void UpdateVertexArray(vertexarray_t& vertex_array, size_t data_size, void* data) {
    glBindBuffer(GL_ARRAY_BUFFER, vertex_array.gl_vertex_buffer);
    glBufferData(GL_ARRAY_BUFFER, data_size, data, GL_DYNAMIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

}