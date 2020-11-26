#ifndef OBJECT_H
#define OBJECT_H

#include <cglm/cglm.h>
#include <GL/gl.h>

#include "transformable.h"
//#include "app.h"
//#include "window.h"
#include "mesh.h"

struct App;
struct Window;
//typedef struct Mesh Mesh;

typedef struct Object {
    mat4 transform;
    vec4 position;
    vec3 animation;
    Mesh *mesh;
    GLuint *shader;
    Transformable transformable;
} Object;

Object object_init(Mesh *mesh, GLuint *shader);
void object_draw(struct App *app, struct Window *win, Object *object);

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


#endif // OBJECT_H
