#ifndef OBJECT_H
#define OBJECT_H

#include <cglm/cglm.h>
#include <GL/gl.h>

#include "transformable.h"

typedef struct App App;
typedef struct Window Window;
typedef struct Mesh Mesh;

typedef struct Object {
    mat4 transform;
    vec4 position;
    vec3 animation;
    Mesh *mesh;
    GLuint *shader;
    Transformable transformable;
} Object;

Object object_init(Mesh *mesh, GLuint *shader);
void object_draw(App *app, Window *win, Object *object);

void object_translate(Object* object, vec3 distances);
void object_translate_transformable(
        void* object, vec3 distances, float delta_time);

void object_rotate(Object* object, float angle, vec3 axis);
void object_rotate_all(Object* object, vec3 angles);
void object_rotate_transformable(
        void* object, vec3 axis, float delta_time);

void object_get_position(Object* object, vec4 dest);

void object_animate(Object* object, float step);
void object_animate_transformable(
        void* object, vec3 angles, float delta_time);


#endif // OBJECT_H
