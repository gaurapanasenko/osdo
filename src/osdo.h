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

#define GAP(i, s)  char gap_##i [s];

#define max(a,b) \
  ({ __typeof__ (a) _a = (a); \
      __typeof__ (b) _b = (b); \
    _a > _b ? _a : _b; })

enum coord_enum {X = 0, Y = 1, Z = 2};

#endif // OSDO_H
