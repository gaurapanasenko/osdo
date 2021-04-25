#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H

#include "osdo.h"
#include "glbindable.h"

class Renderbuffer;
class Texture;

class Framebuffer : public GlBindable
{
    GLuint _generate() const override;
    virtual void _bind(const GLuint id, GLenum target) const override;
    virtual GLenum _default() const override;
public:
    Framebuffer();
    ~Framebuffer() override;

    bool check(GLenum target = GL_FRAMEBUFFER);

    void tex_2d_multisample(GLsizei size[2], const Texture& texture);

    void tex_2d(GLsizei size[2], const Texture& texture);

    void renderbuffer(const Renderbuffer& rb, GLenum target) const;

    void rb_color_multisample(GLsizei size[2], const Renderbuffer& rb) const;

    void rb_depth_multisample(GLsizei size[2], const Renderbuffer& rb) const;

    void rb_color(GLsizei size[2], const Renderbuffer& rb) const;

    void rb_depth(GLsizei size[2], const Renderbuffer& rb) const;
};

#endif // FRAMEBUFFER_H
