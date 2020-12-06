#ifndef BIJECTIVE_H
#define BIJECTIVE_H

#include "osdo.h"

typedef union __attribute__((transparent_union)) bijective_t {
   struct Object *object;
   struct Camera *camera;
} bijective_t;

typedef void (*get_position_cb)(
        bijective_t bijective, vec4 **position);
typedef void (*get_mat4_cb)(
        bijective_t bijective, mat4 matrix);
typedef void (*translate_cb)(
        bijective_t bijective, vec3 distances, float delta_time);
typedef void (*rotate_cb)(
        bijective_t bijective, enum coord_enum coord, float delta_time);
typedef void (*rotate_all_cb)(
        bijective_t bijective, vec3 angles);
typedef void (*get_animation_cb)(
        bijective_t bijective, vec3 **animation);
typedef void (*set_animation_cb)(
        bijective_t bijective, vec3 angles, float delta_time);

typedef struct BijectiveType {
    get_position_cb get_position;
    get_mat4_cb get_mat4;
    translate_cb translate;
    rotate_cb rotate;
    rotate_all_cb rotate_all;
    get_animation_cb get_animation;
    set_animation_cb set_animation;
} BijectiveType;

typedef struct Bijective {
    bijective_t bijective;
    const BijectiveType *type;
} Bijective;

void bijective_get_position(
        Bijective bijective, vec4 **position);

void bijective_get_mat4(
        Bijective bijective, mat4 matrix);

void bijective_translate(
        Bijective bijective, vec3 distances, float delta_time);

void bijective_rotate(
        Bijective bijective, enum coord_enum coord, float delta_time);

void bijective_rotate_all(
        Bijective bijective, vec3 angles);

void bijective_get_animation(
        Bijective bijective, vec3 **animation);

void bijective_set_animation(
        Bijective bijective, vec3 angles, float delta_time);

#endif // BIJECTIVE_H
