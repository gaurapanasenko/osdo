#include "glbinder.h"
#include "glbindable.h"


GlBinder::GlBinder(const GlBindable &bindable, GLenum target)
    : bindable(bindable), target(target) {
    bindable.bind(target);
}

GlBinder::~GlBinder() {bindable.unbind(target);}
