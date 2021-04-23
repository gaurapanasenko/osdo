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

    void make_multisample(GLsizei size[2], GLenum target) const;

    void make(GLsizei size[2], GLenum target) const;
};

#endif // RENDERBUFFER_H
