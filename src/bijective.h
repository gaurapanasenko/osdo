#ifndef BIJECTIVE_H
#define BIJECTIVE_H

#include "osdo.h"

struct BijectiveVtbl;

typedef struct Bijective {
    const struct BijectiveVtbl *vtbl;
} Bijective;

typedef union __attribute__((transparent_union)) bijective_t {
    Bijective *self;
    void *other;
    struct Object *object;
    struct Camera *camera;
} bijective_t;

typedef struct BijectiveVtbl {
    void (*get_position)(
            bijective_t self, vec4 **position);
    void (*get_mat4)(
            bijective_t self, mat4 matrix);
    void (*translate)(
            bijective_t self, vec3 distances, float delta_time);
    void (*rotate)(
            bijective_t self, enum coord_enum coord, float delta_time);
    void (*rotate_all)(
            bijective_t self, vec3 angles);
    void (*get_animation)(
            bijective_t self, vec3 **animation);
    void (*set_animation)(
            bijective_t self, vec3 angles, float delta_time);
} BijectiveVtbl;

static inline void bijective_get_position(
        Bijective *self, vec4 **position) {
    self->vtbl->get_position((bijective_t){self}, position);
}

static inline void bijective_get_mat4(
        Bijective *self, mat4 matrix) {
    self->vtbl->get_mat4((bijective_t){self}, matrix);
}

static inline void bijective_translate(
        Bijective *self, vec3 distances, float delta_time) {
    self->vtbl->translate((bijective_t){self}, distances, delta_time);
}

static inline void bijective_rotate(
        Bijective *self, enum coord_enum coord, float delta_time) {
    self->vtbl->rotate((bijective_t){self}, coord, delta_time);
}

static inline void bijective_rotate_all(
        Bijective *self, vec3 angles) {
    self->vtbl->rotate_all((bijective_t){self}, angles);
}

static inline void bijective_get_animation(
        Bijective *self, vec3 **animation) {
    self->vtbl->get_animation((bijective_t){self}, animation);
}

static inline void bijective_set_animation(
        Bijective *self, vec3 angles, float delta_time) {
    self->vtbl->set_animation((bijective_t){self}, angles, delta_time);
}

#endif // BIJECTIVE_H
