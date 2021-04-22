#include "glbindable.h"
#include "glbinder.h"

GLuint GlBindable::_generate() const {
    return 0;
}

void GlBindable::_bind(const GLuint, GLenum) const {}

GLenum GlBindable::_default() const {
    return 0;
}

GlBindable::GlBindable() : id(_generate()) {}

GlBindable::GlBindable(const GLuint id) : id(id) {}

GlBindable::~GlBindable() {}

const GLuint &GlBindable::get_id() const
{
    return id;
}

void *GlBindable::get_vid() const
{
    return reinterpret_cast<void*>(static_cast<intptr_t>(id));
}

void GlBindable::bind() const
{
    _bind(id, _default());
}

void GlBindable::bind(GLenum target) const
{
    _bind(id, target);
}

void GlBindable::unbind() const
{
    _bind(0, _default());
}

void GlBindable::unbind(GLenum target) const
{
    _bind(0, target);
}

GlBinder GlBindable::binder() const
{
    return {*this, _default()};
}

GlBinder GlBindable::binder(GLenum target) const
{
    return {*this, target};
}


