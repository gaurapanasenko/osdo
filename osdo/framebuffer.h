#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H

#include "osdo.h"
#include "glbindable.h"
#include "texture.h"
#include "renderbuffer.h"

class Framebuffer : public GlBindable
{
    GLuint _generate() const override;
    virtual void _bind(const GLuint id, GLenum target) const override;
    virtual GLenum _default() const override;
public:
    Framebuffer() : GlBindable(_generate()) {}
    ~Framebuffer() override;
    void tex_2d_multisample(GLsizei size[2], const Texture& texture) {
        texture.make_2d_multisample(size);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, texture.get_id(), 0);
    }

    void tex_2d(GLsizei size[2], const Texture& texture) {
        texture.make_2d(size);
        glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, texture.get_id(), 0);
    }

    void renderbuffer(const Renderbuffer& rb, GLenum target) const {
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, target, GL_RENDERBUFFER, rb.get_id());
    }

    void rb_color_multisample(GLsizei size[2], const Renderbuffer& rb) const {
        rb.make_depth_multisample(size);
        renderbuffer(rb, GL_COLOR_ATTACHMENT0);
    }

    void rb_depth_multisample(GLsizei size[2], const Renderbuffer& rb) const {
        rb.make_depth_multisample(size);
        renderbuffer(rb, GL_DEPTH_STENCIL_ATTACHMENT);
    }
};

#endif // FRAMEBUFFER_H
