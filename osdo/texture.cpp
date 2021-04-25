#include "texture.h"

GLuint Texture::_generate() const
{
    GLuint id;
    glGenTextures(1, &id);
    return id;
}

void Texture::_bind(const GLuint id, GLenum target) const
{
    glBindTexture(target, id);
}

GLenum Texture::_default() const
{
    return GL_TEXTURE_2D_MULTISAMPLE;
}

Texture::~Texture() {
    glDeleteTextures(1, &get_id());
}

void LoadTextureFromFile(const pixel_t (*data)[], int width, int height,
                         const GLuint id)
{
    glBindTexture(GL_TEXTURE_2D, id);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

#if defined(GL_UNPACK_ROW_LENGTH) && !defined(__EMSCRIPTEN__)
    glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
#endif
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA,
                 GL_UNSIGNED_BYTE, data);
}

void Texture::update(const Image &image) const {
    LoadTextureFromFile(image.data.get(), image.width, image.height, get_id());
}

void Texture::make_2d_multisample(GLsizei size[]) const {
    GlBinder b = binder(GL_TEXTURE_2D_MULTISAMPLE);
    glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, 4, GL_RGB,
                            size[0], size[1], GL_TRUE);
}

void Texture::make_2d(GLsizei size[]) const {
    GlBinder b = binder(GL_TEXTURE_2D);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, size[0], size[1], 0, GL_RGB,
            GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
}
