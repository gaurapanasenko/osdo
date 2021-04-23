#ifndef OSDO_H
#define OSDO_H

#include <cglm/cglm.h>
#include <GL/glew.h>
#include <GL/gl.h>

/*#define max(a,b) \
  ({ __typeof__ (a) _a = (a); \
      __typeof__ (b) _b = (b); \
    _a > _b ? _a : _b; })*/

enum coord_enum {X = 0, Y = 1, Z = 2};

#ifdef __GNUC__
#define UNUSED __attribute__ ((unused))
#else
#define UNUSED
#endif

//#define EASTL_VARIABLE_TEMPLATES_ENABLED 1
void* __cdecl operator new[](size_t size, const char* name, int flags,
unsigned debugFlags, const char* file, int line);
void* operator new[](size_t size, size_t alignment, size_t alignmentOffset,
const char* pName, int flags, unsigned debugFlags, const char* file, int line);

#endif // OSDO_H
