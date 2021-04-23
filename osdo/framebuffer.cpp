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

Framebuffer::Framebuffer() : GlBindable(_generate()) {}

Framebuffer::~Framebuffer() {
    glDeleteFramebuffers(1, &get_id());
}

bool Framebuffer::check(GLenum target) {
    return glCheckFramebufferStatus(target) == GL_FRAMEBUFFER_COMPLETE;
}

void Framebuffer::tex_2d_multisample(GLsizei size[2], const Texture &texture) {
    texture.make_2d_multisample(size);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                           GL_TEXTURE_2D_MULTISAMPLE, texture.get_id(), 0);
}

void Framebuffer::tex_2d(GLsizei size[2], const Texture &texture) {
    texture.make_2d(size);
    glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                         texture.get_id(), 0);
}

void Framebuffer::renderbuffer(const Renderbuffer &rb, GLenum target) const {
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, target,
                              GL_RENDERBUFFER, rb.get_id());
}

void Framebuffer::rb_color_multisample(
        GLsizei size[2], const Renderbuffer &rb) const {
    rb.make_multisample(size, GL_RGB);
    renderbuffer(rb, GL_COLOR_ATTACHMENT0);
}

void Framebuffer::rb_depth_multisample(
        GLsizei size[2], const Renderbuffer &rb) const {
    rb.make_multisample(size, GL_DEPTH32F_STENCIL8);
    renderbuffer(rb, GL_DEPTH_STENCIL_ATTACHMENT);
}

void Framebuffer::rb_color(GLsizei size[2], const Renderbuffer &rb) const {
    rb.make(size, GL_RGB);
    renderbuffer(rb, GL_COLOR_ATTACHMENT0);
}


void Framebuffer::rb_depth(GLsizei size[2], const Renderbuffer &rb) const {
    rb.make(size, GL_DEPTH32F_STENCIL8);
    renderbuffer(rb, GL_DEPTH_STENCIL_ATTACHMENT);
}
