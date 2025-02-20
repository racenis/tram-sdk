// Tramway Drifting and Dungeon Exploration Simulator SDK Runtime

#include <render/opengl/texture.h>

#include <config.h>

#ifndef _WIN32
    #include <GL/gl.h>
    #include <GLES3/gl3.h>
#else
    #include <glad.h>
#endif

using namespace tram;

namespace tram::Render::API {

texturehandle_t CreateTexture(ColorMode color_mode, TextureFilter texture_filter, uint32_t width, uint32_t height, void* data) {
    uint32_t texture;
    uint32_t opengl_tex_format;
    
    switch (color_mode) {
        case COLORMODE_R:
            opengl_tex_format = GL_RED;
            break;
        case COLORMODE_RG:
            opengl_tex_format = GL_RG;
            break;
        case COLORMODE_RGB:
            opengl_tex_format = GL_RGB;
            break;
        case COLORMODE_RGBA:
            opengl_tex_format = GL_RGBA;
    }
    
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    switch (texture_filter) {
        case TEXTUREFILTER_NEAREST:
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            break;
        case TEXTUREFILTER_LINEAR:
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            break;
        case TEXTUREFILTER_LINEAR_MIPMAPPED:
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            break;
    }

    assert(data);

    glTexImage2D(GL_TEXTURE_2D, 0, opengl_tex_format, width, height, 0, opengl_tex_format, GL_UNSIGNED_BYTE, data);
        
    glGenerateMipmap(GL_TEXTURE_2D);
    
    return {.gl_texture_handle = texture};
}

}