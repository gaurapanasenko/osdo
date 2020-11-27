#ifndef TRANSFORMABLE_H
#define TRANSFORMABLE_H

#include <cglm/cglm.h>

typedef void (*get_position_cb)(void* object, vec4 **position);
typedef void (*translate_cb)(void *object, vec3 distances, float delta_time);
typedef void (*rotate_cb)(void* object, vec3 axis, float delta_time);
typedef void (*rotate_all_cb)(void* object, vec3 angles);
typedef void (*get_animation_cb)(void* object, vec3 **animation);
typedef void (*set_animation_cb)(void* object, vec3 angles, float delta_time);

typedef struct Transformable {
    get_position_cb get_position;
    translate_cb translate;
    rotate_cb rotate;
    rotate_all_cb rotate_all;
    get_animation_cb get_animation;
    set_animation_cb set_animation;
} Transformable;

#endif // TRANSFORMABLE_H
