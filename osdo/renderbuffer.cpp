#include "renderbuffer.h"

GLuint Renderbuffer::_generate() const
{
    GLuint id;
    glGenRenderbuffers(1, &id);
    return id;
}

void Renderbuffer::_bind(const GLuint id, GLenum target) const
{
    glBindRenderbuffer(target, id);
}

GLenum Renderbuffer::_default() const
{
    return GL_RENDERBUFFER;
}

Renderbuffer::~Renderbuffer() {
    glDeleteRenderbuffers(1, &get_id());
}
