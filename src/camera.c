#include "camera.h"
#include "conf.h"

void camera_get_direction(Camera* camera, vec4 dest) {
    mat4 matrix;
    camera_get_rotation_inv_mat4(camera, matrix);
    glm_mat4_mulv(matrix, CAMERA_DIRECTION, dest);
    //glm_vec3_copy(camera->direction, dest);
    //dest[3] = 1.f;
}

void camera_get_mat4(Camera *camera, mat4 dest) {
    camera_get_rotation_mat4(camera, dest);
    glm_translate(dest, camera->position);
    //glm_look(camera->position, camera->direction, GLM_YUP, dest);
}

void camera_get_rotation_mat4(Camera *camera, mat4 dest) {
    //glm_lookat(GLM_VEC3_ZERO, camera->direction, GLM_YUP, dest);
    //glm_quat_mat4(camera->rotation, dest);
    glm_mat4_copy(camera->rotation, dest);
    glm_mat4_inv(dest, dest);
    /*glm_rotate_make(dest, camera->rotation[1], GLM_YUP);
    vec4 axis;
    glm_vec3_copy(GLM_XUP, axis);
    axis[3] = 1;
    glm_mat4_mulv(dest, axis, axis);
    glm_rotate(dest, camera->rotation[0], GLM_XUP);*/
    //glm_rotate(dest, camera->rotation[2], GLM_ZUP);
}

void camera_get_rotation_inv_mat4(Camera *camera, mat4 dest) {
    //vec3 inv;
    //glm_vec3_inv_to(camera->direction, inv);
    //glm_lookat(GLM_VEC3_ZERO, camera->direction, GLM_YUP, dest);
    //glm_quat_mat4(camera->rotation, dest);
    //glm_mat4_inv(dest, dest);
    glm_mat4_copy(camera->rotation, dest);
    //glm_mat4_inv(dest, dest);
    //camera_get_rotation_mat4(camera, dest);
    //glm_mat4_inv(dest, dest);
}

void camera_get_position_bijective(Camera *camera, vec4 **position) {
    *position = &camera->position;
}

void camera_translate(Camera *camera, vec3 distances) {
    glm_vec3_add(camera->position, distances, camera->position);
}

void camera_translate_bijective(
        Camera *camera, vec3 distances, float delta_time) {
    vec3 new_distances = GLM_VEC3_ZERO_INIT;
    mat4 rotation;
    camera_get_rotation_inv_mat4(camera, rotation);

    glm_vec3_muladds(distances, -OBJECT_MOVE_SPEED * delta_time,
                     new_distances);
    glm_vec3_rotate_m4(rotation, new_distances, new_distances);
    camera_translate(camera, new_distances);
}

void camera_rotate(Camera *camera, float angle, enum coord_enum coord) {
    //glm_vec3_muladds(axis, angle, camera->rotation);
    //glm_rotate(camera->rotation, -angle, axis);
    switch (coord) {
    case X:
        glm_rotate_x(camera->rotation, angle, camera->rotation);
        //camera->rotation[1] -= angle;
        break;
    case Y:
        //camera->rotation[2] -= angle;
        glm_rotate_y(camera->rotation, angle, camera->rotation);
        break;
    case Z:
        //camera->rotation[3] -= angle;
        glm_rotate_z(camera->rotation, angle, camera->rotation);
        break;
    }
    if (glm_vec3_dot(camera->rotation[2], GLM_XUP) > 0.1f) {
        glm_cross(camera->rotation[2], GLM_XUP, camera->rotation[1]);
        glm_cross(camera->rotation[1], camera->rotation[2], camera->rotation[0]);
        glm_normalize(camera->rotation[0]);
        glm_normalize(camera->rotation[1]);
    }
    //glm_quat_normalize(camera->rotation);
    //glm_mat4_copy(camera->rotation, dest);
    //glm_mat4_inv(dest, dest);
    //glm_vec3_rotate_m4(dest, coord, a);
    //glm_rotate(camera->rotation, angle, a);
    /*glm_vec3_muladds(axis, angle, camera->rotation);
    mat4 buf;
    camera_get_rotation_mat4(camera, buf);
    float yaw = camera->rotation[1], pitch = camera->rotation[0];
    //vec3 a;
    //glm_cross(camera->direction, axis, a);
    //glm_vec3_rotate(camera->direction, -angle, axis);
    //mat4 buf;
    //glm_rotate_make(buf, angle, axis);
    //glm_mat4_mul(camera->rotation, buf, camera->rotation);
    //glm_rotate(camera->rotation, angle, axis);
    vec3 direction = {
        cosf(yaw) * cosf(pitch), sinf(pitch), sinf(yaw) * cosf(pitch)};
    glm_normalize(direction);
    glm_vec3_copy(direction, camera->direction);*/
}

void camera_rotate_all(Camera *camera, vec3 angles) {
    camera_rotate(camera, angles[0], X);
    camera_rotate(camera, angles[1], Y);
    camera_rotate(camera, angles[2], Z);
}

void camera_rotate_bijective(
        Camera *camera, enum coord_enum coord, float delta_time) {
    camera_rotate(camera, -OBJECT_ROTATE_SPEED * delta_time, coord);
}

void camera_get_animation(Camera *camera, vec3 **animation) {
    *animation = &camera->animation;
}

void camera_set_animation(
        Camera *camera, vec3 angles, float delta_time) {
    vec3 animation;
    glm_vec3_muladds(angles, delta_time, animation);
    glm_vec3_add(camera->animation, animation,
                 camera->animation);
}

void camera_rotate_all_bijective(Camera *camera, vec3 angles) {
    camera_rotate_all(camera, angles);
}
