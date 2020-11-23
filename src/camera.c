#include "camera.h"
#include "conf.h"

void camera_init(Camera* camera) {
    *camera = (Camera){
        GLM_QUAT_IDENTITY_INIT,
        GLM_VEC3_ZERO_INIT,
        GLM_VEC3_ZERO_INIT,
        {
            camera_translate_transformable,
            camera_rotate_transformable,
            camera_animate_transformable
        }
    };
}

void camera_get_direction(Camera* camera, vec4 dest) {
    mat4 view;
    glm_quat_rotate(GLM_MAT4_IDENTITY, camera->rotation, view);
    glm_mat4_mulv(view, CAMERA_DIRECTION, dest);
}

void camera_get_mat4(Camera* camera, mat4 dest) {
    glm_quat_mat4(camera->rotation, dest);
    glm_translate(dest, camera->position);
}

void camera_translate(Camera* camera, vec3 distances) {
    glm_vec3_add(camera->position, distances, camera->position);
}

void camera_translate_transformable(
        void* camera, vec3 distances, float delta_time) {
    vec3 new_distances = GLM_VEC3_ZERO_INIT;
    versor quat;
    glm_vec3_muladds(distances, -OBJECT_MOVE_SPEED * delta_time,
                     new_distances);
    glm_quat_inv(((Camera*)camera)->rotation, quat);
    glm_quat_rotatev(quat, new_distances, new_distances);
    camera_translate((Camera*)camera, new_distances);
}

void camera_rotate(Camera* camera, float angle, vec3 axis) {
    versor qantnerion;
    glm_quatv(qantnerion, angle, axis);
    glm_quat_mul(qantnerion, camera->rotation, camera->rotation);
}

void camera_rotate_all(Camera *camera, vec3 angles) {
    camera_rotate(camera, angles[0], GLM_XUP);
    camera_rotate(camera, angles[1], GLM_YUP);
    camera_rotate(camera, angles[2], GLM_ZUP);
}

void camera_rotate_transformable(
        void* camera, vec3 axis, float delta_time) {
    camera_rotate((Camera*)camera, OBJECT_ROTATE_SPEED * delta_time,
                  axis);
}

void camera_animate_transformable(
        void* camera, vec3 angles, float delta_time) {
    vec3 animation;
    glm_vec3_muladds(angles, delta_time, animation);
    glm_vec3_add(((Camera*)camera)->animation, animation,
                 ((Camera*)camera)->animation);
}
