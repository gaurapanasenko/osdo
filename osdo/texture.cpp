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
