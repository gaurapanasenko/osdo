#ifndef TRANSFORMABLE_H
#define TRANSFORMABLE_H

#include <cglm/cglm.h>

typedef void (*translate_cb)(void *object, vec3 distances, float delta_time);
typedef void (*rotate_cb)(void* object, vec3 axis, float delta_time);
typedef void (*set_animation_cb)(void* object, vec3 angles, float delta_time);

typedef struct Transformable {
    translate_cb translate;
    rotate_cb rotate;
    set_animation_cb set_animation;
} Transformable;

#endif // TRANSFORMABLE_H
