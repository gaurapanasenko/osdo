#ifndef OBJECT_H
#define OBJECT_H

#include <cglm/cglm.h>
#include <GL/gl.h>

#define CAMERA_DIRECTION_INIT {0.0f, 0.0f, -1.0f, 0.0f}
#define CAMERA_DIRECTION ((vec4)CAMERA_DIRECTION_INIT)

typedef struct App App;
typedef struct Window Window;
typedef struct Mesh Mesh;

typedef struct Object {
    mat4 transform;
    vec4 position;
    vec3 animation;
    Mesh *mesh;
    GLuint *shader;
} Object;

Object object_init(Mesh *mesh, GLuint *shader);
void object_draw(App *app, Window *win, Object *object);

void object_translate(Object* object, vec3 distances);
void object_translate_camera(Object* object, vec3 distances);
void object_translate_speed(Object* object, vec3 distances, int active, float s);

void object_rotate(Object* object, float angle, vec3 axis);
void object_rotate_speed(Object* object, float angle, vec3 axis);
void object_rotate_all(Object* object, vec3 angles);

void object_get_camera_position(Object* object, vec4 dest);
void object_get_camera_direction(Object* object, vec4 dest);

void object_get_position(Object* object, vec4 dest);

void object_animate(Object* object, float deltaTime);


#endif // OBJECT_H
