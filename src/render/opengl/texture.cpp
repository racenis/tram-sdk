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
    texturehandle_t texture;
    
    
    glGenTextures(1, &texture.gl_texture_handle);
    glBindTexture(GL_TEXTURE_2D, texture.gl_texture_handle);

    // do we want to expose this in the API?
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    SetTextureFilter(texture, texture_filter);
    SetTextureImage(texture, color_mode, width, height, data);
    
    return texture;
}

void SetTextureFilter(texturehandle_t texture, TextureFilter texture_filter) {
    glBindTexture(GL_TEXTURE_2D, texture.gl_texture_handle);
    
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
}

void SetTextureImage(texturehandle_t texture, ColorMode color_mode, uint32_t width, uint32_t height, void* data) {
    uint32_t opengl_tex_format;
    
    glBindTexture(GL_TEXTURE_2D, texture.gl_texture_handle);
    
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

    assert(data);

    glTexImage2D(GL_TEXTURE_2D, 0, opengl_tex_format, width, height, 0, opengl_tex_format, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
}

void YeetTexture(texturehandle_t texture) {
    glDeleteTextures(1, &texture.gl_texture_handle);
}

}