#ifndef RENDERBUFFER_H
#define RENDERBUFFER_H

#include "osdo.h"
#include "glbindable.h"
#include "glbinder.h"

class Renderbuffer : public GlBindable
{
    GLuint _generate() const override;
    virtual void _bind(const GLuint id, GLenum target) const override;
    virtual GLenum _default() const override;
public:
    Renderbuffer() : GlBindable(_generate()) {}
    ~Renderbuffer() override;

    void make_multisample(GLsizei size[2], GLenum target) const {
        GlBinder b = binder();
        glRenderbufferStorageMultisample(GL_RENDERBUFFER, 4, target, size[0], size[1]);
    }

    void make_color_multisample(GLsizei size[2]) const {
        make_multisample(size, GL_RGB);
    }

    void make_depth_multisample(GLsizei size[2]) const {
        make_multisample(size, GL_DEPTH24_STENCIL8);
    }
};

#endif // RENDERBUFFER_H
