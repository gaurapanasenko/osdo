#ifndef OSDO_H
#define OSDO_H

#include <cglm/cglm.h>
#include <GL/glew.h>
#include <GL/gl.h>

/*#define max(a,b) \
  ({ __typeof__ (a) _a = (a); \
      __typeof__ (b) _b = (b); \
    _a > _b ? _a : _b; })*/

/**
 * @brief Координатні осі.
 */
enum coord_enum {
    X = 0, /**< Ввісь X */
    Y = 1, /**< Ввісь Y */
    Z = 2, /**< Ввісь Z */
};

#ifdef __GNUC__
#define UNUSED __attribute__ ((unused))
#else
#define UNUSED
#endif

/**
 * @brief Перевантажений оператор `new` для EASTL
 */
void* operator new[](size_t size, const char* name, int flags,
unsigned debugFlags, const char* file, int line);

/**
 * @brief Перевантажений оператор `new` для EASTL
 */
void* operator new[](size_t size, size_t alignment, size_t alignmentOffset,
const char* pName, int flags, unsigned debugFlags, const char* file, int line);

/**
 * @brief Початкова позиція камери.
 */
extern vec3 BASIS0POS;
//extern vec3 BASIS1POS;
//extern vec3 BASIS2POS;

//extern vec3 BASIS1ROT;
//extern vec3 BASIS2ROT;

#endif // OSDO_H
