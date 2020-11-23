#ifndef CAMERA_H
#define CAMERA_H

#include <cglm/cglm.h>
#include <GL/gl.h>

#include "transformable.h"

#define CAMERA_DIRECTION_INIT {0.0f, 0.0f, -1.0f, 0.0f}
#define CAMERA_DIRECTION ((vec4)CAMERA_DIRECTION_INIT)

typedef struct App App;
typedef struct Window Window;
typedef struct Mesh Mesh;

typedef struct Camera {
    versor rotation;
    vec4 position;
    vec3 animation;
    Transformable transformable;
} Camera;

void camera_init(Camera* camera);

void camera_get_direction(Camera* camera, vec4 dest);
void camera_get_mat4(Camera* camera, mat4 dest);

void camera_translate(Camera* camera, vec3 distances);
void camera_translate_transformable(
        void* camera, vec3 distances, float delta_time);

void camera_rotate(Camera* camera, float angle, vec3 axis);
void camera_rotate_all(Camera *camera, vec3 angles);
void camera_rotate_transformable(
        void* camera, vec3 axis, float delta_time);

void camera_animate_transformable(
        void* camera, vec3 angles, float delta_time);

#endif // CAMERA_H
