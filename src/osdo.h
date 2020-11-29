#ifndef OSDO_H
#define OSDO_H

#include <cglm/cglm.h>
#include <GL/glew.h>
#include <GL/gl.h>
#include <uthash.h>
#include <utarray.h>

#define for_each_utarr(type, item, list) \
    for(type *item = (type*)(void*)utarray_front(list); item != NULL; \
        item = (type*)(void*)utarray_next(list, item))

enum coord_enum {
    X, Y, Z,
};

#endif // OSDO_H
