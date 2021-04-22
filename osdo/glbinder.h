#ifndef GLBINDER_H
#define GLBINDER_H

#include "glbindable.h"

class GlBinder
{
    const GlBindable& bindable;
    const GLenum target;
public:
    GlBinder(const GlBindable& bindable, GLenum target);
    ~GlBinder();
};

#endif // GLBINDER_H
