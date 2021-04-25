#ifndef TEXTURE_H
#define TEXTURE_H

#include "osdo.h"
#include "glbindable.h"
#include "glbinder.h"
#include "image.h"

class Texture : public GlBindable
{
    GLuint _generate() const override;
    virtual void _bind(const GLuint id, GLenum target) const override;
    virtual GLenum _default() const override;
public:
    Texture() : GlBindable(_generate()) {}
    ~Texture() override;

    void update(const Image& image) const;

    void make_2d_multisample(GLsizei size[2]) const;

    void make_2d(GLsizei size[2]) const;
};

#endif // TEXTURE_H