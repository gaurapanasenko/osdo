#include "renderbuffer.h"
#include "glbinder.h"

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

void Renderbuffer::make_multisample(GLsizei size[2], GLenum target) const {
    GlBinder b = binder();
    glRenderbufferStorageMultisample(
                GL_RENDERBUFFER, 4, target, size[0], size[1]);
}

void Renderbuffer::make(GLsizei size[2], GLenum target) const
{
    GlBinder b = binder();
    glRenderbufferStorage(GL_RENDERBUFFER, target, size[0], size[1]);

}
