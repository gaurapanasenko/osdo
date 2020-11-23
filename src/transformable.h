#ifndef TRANSFORMABLE_H
#define TRANSFORMABLE_H

#include <cglm/cglm.h>

typedef void (*translate_t)(void *object, vec3 distances, float delta_time);
typedef void (*rotate_t)(void* object, vec3 axis, float delta_time);
typedef void (*animate_t)(void* object, vec3 angles, float delta_time);

typedef struct Transformable {
    translate_t translate;
    rotate_t rotate;
    animate_t animate;
} Transformable;

#endif // TRANSFORMABLE_H
