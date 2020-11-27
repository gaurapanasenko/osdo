#include "camera.h"
#include "conf.h"

void camera_get_direction(Camera* camera, vec4 dest) {
    mat4 matrix;
    glm_rotate_make(matrix, camera->rotation[0], GLM_XUP);
    glm_rotate(matrix, camera->rotation[1], GLM_YUP);
    glm_rotate(matrix, camera->rotation[2], GLM_ZUP);
    glm_mat4_mulv(matrix, CAMERA_DIRECTION, dest);
}

void camera_get_mat4(void *camera, mat4 dest) {
    Camera *cam = (Camera*)camera;
    glm_rotate_make(dest, cam->rotation[0], GLM_XUP);
    glm_rotate(dest, cam->rotation[1], GLM_YUP);
    glm_rotate(dest, cam->rotation[2], GLM_ZUP);
    glm_translate(dest, cam->position);
}

void camera_get_position_transformable(void* camera, vec4 **position) {
    *position = &((Camera*)camera)->position;
}

void camera_translate(Camera* camera, vec3 distances) {
    glm_vec3_add(camera->position, distances, camera->position);
}

void camera_translate_transformable(
        void* camera, vec3 distances, float delta_time) {
    Camera *cam = (Camera*)camera;
    vec3 new_distances = GLM_VEC3_ZERO_INIT;
    mat4 rotation;
    glm_rotate_make(rotation, -cam->rotation[2], GLM_ZUP);
    glm_rotate(rotation, -cam->rotation[1], GLM_YUP);
    glm_rotate(rotation, -cam->rotation[0], GLM_XUP);

    glm_vec3_muladds(distances, -OBJECT_MOVE_SPEED * delta_time,
                     new_distances);
    glm_vec3_rotate_m4(rotation, new_distances, new_distances);
    camera_translate(cam, new_distances);
}

void camera_rotate(Camera* camera, float angle, vec3 axis) {
    glm_vec3_muladds(axis, angle, camera->rotation);
}

void camera_rotate_all(Camera *camera, vec3 angles) {
    glm_vec3_add(camera->rotation, angles, camera->rotation);
}

void camera_rotate_transformable(
        void* camera, vec3 axis, float delta_time) {
    camera_rotate((Camera*)camera, OBJECT_ROTATE_SPEED * delta_time,
                  axis);
}

void camera_get_animation(void* camera, vec3 **animation) {
    *animation = &((Camera*)camera)->animation;
}

void camera_set_animation(
        void* camera, vec3 angles, float delta_time) {
    vec3 animation;
    glm_vec3_muladds(angles, delta_time, animation);
    glm_vec3_add(((Camera*)camera)->animation, animation,
                 ((Camera*)camera)->animation);
}

void camera_rotate_all_transformable(void* camera, vec3 angles) {
    camera_rotate_all((Camera*)camera, angles);
}
