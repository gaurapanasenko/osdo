#ifndef GLBINDABLE_H
#define GLBINDABLE_H

#include "osdo.h"

class GlBinder;

class GlBindable
{
    const GLuint id;
protected:
    virtual GLuint _generate() const;
    virtual void _bind(const GLuint id, GLenum target) const;
    virtual GLenum _default() const;
public:
    GlBindable();
    GlBindable(const GLuint id);
    virtual ~GlBindable();

    GlBindable& operator=(const GlBindable&) = delete;
    GlBindable& operator=(GlBindable&&) = delete;

    const GLuint& get_id() const;
    void* get_vid() const;

    void bind() const;
    void bind(GLenum target) const;
    void unbind() const;
    void unbind(GLenum target) const;

    GlBinder binder() const;
    GlBinder binder(GLenum target) const;
};

#endif // GLBINDABLE_H
