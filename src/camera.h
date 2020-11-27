#ifndef CAMERA_H
#define CAMERA_H

#include <cglm/cglm.h>
#include <GL/gl.h>

#include "transformable.h"

#define CAMERA_DIRECTION_INIT {0.0f, 0.0f, -1.0f, 0.0f}
#define CAMERA_DIRECTION ((vec4)CAMERA_DIRECTION_INIT)

typedef struct Camera {
    vec3 rotation;
    vec4 position;
    vec3 animation;
    Transformable transformable;
} Camera;

void camera_get_direction(Camera* camera, vec4 dest);
void camera_get_mat4(Camera* camera, mat4 dest);
void camera_get_position_transformable(
        void* camera, vec4 **position);

void camera_translate(Camera* camera, vec3 distances);
void camera_translate_transformable(
        void* camera, vec3 distances, float delta_time);

void camera_rotate(Camera* camera, float angle, vec3 axis);
void camera_rotate_all(Camera *camera, vec3 angles);
void camera_rotate_transformable(
        void* camera, vec3 axis, float delta_time);
void camera_rotate_all_transformable(void* camera, vec3 angles);

void camera_get_animation(void* camera, vec3 **animation);
void camera_set_animation(void* camera, vec3 angles, float delta_time);

#define CAMERA_INIT {\
    GLM_VEC3_ZERO_INIT,\
    GLM_VEC4_BLACK_INIT,\
    GLM_VEC3_ZERO_INIT,\
    {\
        camera_get_position_transformable,\
        camera_translate_transformable,\
        camera_rotate_transformable,\
        camera_rotate_all_transformable,\
        camera_get_animation,\
        camera_set_animation\
    }}
#define CAMERA ((Camera)CAMERA_INIT)

#define CAMERAINIT_EMPTY CAMERA_INIT(NULL, NULL)

#endif // CAMERA_H
