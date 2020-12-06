#ifndef CAMERA_H
#define CAMERA_H

#include "osdo.h"

#include "bijective.h"

#define CAMERA_DIRECTION_INIT {0.0f, 0.0f, -1.0f, 0.0f}
#define CAMERA_DIRECTION ((vec4)CAMERA_DIRECTION_INIT)

typedef struct Camera {
    mat4 rotation;
    vec4 position;
    vec3 animation;
} Camera;

void camera_get_direction(Camera* camera, vec4 dest);
void camera_get_mat4(Camera *camera, mat4 dest);
void camera_get_rotation_mat4(Camera *camera, mat4 dest);
void camera_get_rotation_inv_mat4(Camera *camera, mat4 dest);
void camera_get_position_bijective(
        Camera *camera, vec4 **position);

void camera_translate(Camera* camera, vec3 distances);
void camera_translate_bijective(
        Camera *camera, vec3 distances, float delta_time);

void camera_rotate(Camera* camera, float angle, enum coord_enum coord);
void camera_rotate_all(Camera* camera, vec3 angles);
void camera_rotate_bijective(
        Camera *camera, enum coord_enum coord, float delta_time);
void camera_rotate_all_bijective(Camera *camera, vec3 angles);

void camera_get_animation(Camera *camera, vec3 **animation);
void camera_set_animation(Camera *camera, vec3 angles, float delta_time);

static const BijectiveType camera_bijective = {
    camera_get_position_bijective,
    camera_get_mat4,
    camera_translate_bijective,
    camera_rotate_bijective,
    camera_rotate_all_bijective,
    camera_get_animation,
    camera_set_animation
};

#define CAMERA_INIT {GLM_MAT4_IDENTITY_INIT, GLM_VEC4_BLACK_INIT, \
    GLM_VEC3_ZERO_INIT}
#define CAMERA ((Camera)CAMERA_INIT)

#define CAMERAINIT_EMPTY CAMERA_INIT(NULL, NULL)

#endif // CAMERA_H
