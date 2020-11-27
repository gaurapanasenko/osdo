#ifndef OBJECT_H
#define OBJECT_H

#include <cglm/cglm.h>
#include <GL/gl.h>
#include "utarray.h"

#include "transformable.h"
#include "mesh.h"
#include "shader.h"

typedef struct Object {
    mat4 transform;
    vec4 position;
    vec3 animation;
    Mesh *mesh;
    Shader *shader;
    Transformable transformable;
} Object;

Object object_init(Mesh *mesh, Shader *shader);
void object_init_empty(void *object);
void object_draw(Object *object, mat4 mat4buf, GLdouble delta_time);

void object_translate(Object* object, vec3 distances);
void object_translate_transformable(
        void* object, vec3 distances, float delta_time);

void object_rotate(Object* object, float angle, vec3 axis);
void object_rotate_all(Object* object, vec3 angles);
void object_rotate_transformable(
        void* object, vec3 axis, float delta_time);

void object_get_position(Object* object, vec4 dest);

void object_animate(Object* object, float step);
void object_set_animation_transformable(
        void* object, vec3 angles, float delta_time);

#define OBJECT_INIT(mesh, shader) {\
    GLM_MAT4_IDENTITY_INIT,\
    GLM_VEC4_BLACK_INIT,\
    GLM_VEC3_ZERO_INIT, mesh, shader,\
    {\
        object_translate_transformable,\
        object_rotate_transformable,\
        object_set_animation_transformable\
    }}
#define OBJECT(mesh, shader) ((Object)OBJECT_INIT(mesh, shader))

#define OBJECT_INIT_EMPTY OBJECT_INIT(NULL, NULL)
#define OBJECT_EMPTY OBJECT(NULL, NULL)

static const UT_icd object_icd = {
    sizeof(Object), object_init_empty, NULL, NULL
};

#endif // OBJECT_H
