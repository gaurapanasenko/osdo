#ifndef OBJECT_H
#define OBJECT_H

#include "osdo.h"

#include "bijective.h"
#include "model.h"
#include "shader.h"

typedef struct Object {
    Bijective bijective;
    mat4 transform;
    vec4 position;
    vec3 animation;
    Model *model;
    Shader *shader;
} Object;

void object_init(Object *object, Model *model, Shader *shader);
void object_init_empty(void *object);
void object_draw(Object *object, mat4 mat4buf, GLdouble delta_time);

void object_get_position_bijective(
        Object* object, vec4 **position);
void object_get_mat4(Object* object, mat4 dest);

void object_translate(Object* object, vec3 distances);
void object_translate_bijective(
        Object* object, vec3 distances, float delta_time);

void object_rotate(Object* object, float angle, enum coord_enum coord);
void object_rotate_all(Object* object, vec3 angles);
void object_rotate_bijective(
        Object* object, enum coord_enum coord, float delta_time);
void object_rotate_all_bijective(Object* object, vec3 angles);

void object_get_position(Object* object, vec4 dest);

void object_animate(Object* object, float step);
void object_get_animation(Object *object, vec3 **animation);
void object_set_animation(Object *object, vec3 angles, float delta_time);

void object_scale(Object *object, vec3 scale);

static const BijectiveVtbl object_bijective = {
    object_get_position_bijective,
    object_get_mat4,
    object_translate_bijective,
    object_rotate_bijective,
    object_rotate_all_bijective,
    object_get_animation,
    object_set_animation
};

#define OBJECT_INIT(model, shader) {\
    {&object_bijective}, \
    GLM_MAT4_IDENTITY_INIT,\
    GLM_VEC4_BLACK_INIT,\
    GLM_VEC3_ZERO_INIT, model, shader}
#define OBJECT(model, shader) ((Object)OBJECT_INIT(model, shader))

#define OBJECT_INIT_EMPTY OBJECT_INIT(NULL, NULL)
#define OBJECT_EMPTY OBJECT(NULL, NULL)

static const UT_icd object_icd = {
    sizeof(Object), object_init_empty, NULL, NULL
};

#endif // OBJECT_H
