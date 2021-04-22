#include "framebuffer.h"


GLuint Framebuffer::_generate() const
{
    GLuint id;
    glGenFramebuffers(1, &id);
    return id;
}

void Framebuffer::_bind(const GLuint id, GLenum target) const
{
    glBindFramebuffer(target, id);
}

GLenum Framebuffer::_default() const
{
    return GL_FRAMEBUFFER;
}

Framebuffer::~Framebuffer() {
    glDeleteFramebuffers(1, &get_id());
}
