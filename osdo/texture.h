#ifndef TEXTURE_H
#define TEXTURE_H

#include "osdo.h"
#include "glbindable.h"
#include "glbinder.h"

class Texture : public GlBindable
{
    GLuint _generate() const override;
    virtual void _bind(const GLuint id, GLenum target) const override;
    virtual GLenum _default() const override;
public:
    Texture() : GlBindable(_generate()) {}
    ~Texture() override;

    void make_2d_multisample(GLsizei size[2]) const {
        GlBinder b = binder(GL_TEXTURE_2D_MULTISAMPLE);
        glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, 4, GL_RGB, size[0], size[1], GL_TRUE);
    }

    void make_2d(GLsizei size[2]) const {
        GlBinder b = binder(GL_TEXTURE_2D);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, size[0], size[1], 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    }
};

#endif // TEXTURE_H
