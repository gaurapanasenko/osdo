#include "camera.h"
#include "conf.h"

Camera::Camera()
    : rotation GLM_MAT4_IDENTITY_INIT,
      position GLM_VEC4_BLACK_INIT,
      animation GLM_VEC3_ZERO_INIT
{

}

void Camera::get_direction(vec4 dest) {
    mat4 matrix;
    Camera::get_rotation_inv_mat4(matrix);
    glm_mat4_mulv(matrix, CAMERA_DIRECTION, dest);
}

void Camera::get_mat4(mat4 dest) {
    Camera::get_rotation_mat4(dest);
    glm_translate(dest, this->position);
}

void Camera::get_rotation_mat4(mat4 dest) {
    glm_mat4_copy(this->rotation, dest);
    glm_mat4_inv(dest, dest);
}

void Camera::get_rotation_inv_mat4(mat4 dest) {
    glm_mat4_copy(this->rotation, dest);
}

void Camera::get_position(vec4 **position) {
    *position = &this->position;
}

void Camera::translate_camera(vec3 distances) {
    glm_vec3_add(this->position, distances, this->position);
}

void Camera::translate(
        vec3 distances, float delta_time) {
    vec3 new_distances = GLM_VEC3_ZERO_INIT;
    mat4 rotation;
    Camera::get_rotation_inv_mat4(rotation);

    glm_vec3_muladds(distances, -OBJECT_MOVE_SPEED * delta_time,
                     new_distances);
    glm_vec3_rotate_m4(rotation, new_distances, new_distances);
    Camera::translate_camera(new_distances);
}

void Camera::rotate_camera(float angle, enum coord_enum coord) {
    switch (coord) {
    case X: glm_rotate_x(this->rotation, angle, this->rotation); break;
    case Y: glm_rotate_y(this->rotation, angle, this->rotation); break;
    case Z: glm_rotate_z(this->rotation, angle, this->rotation); break;
    }
    /*if (glm_vec3_dot(this->rotation[2], GLM_XUP) > 0.1f) {
        glm_cross(this->rotation[2], GLM_XUP, this->rotation[1]);
        glm_cross(this->rotation[1], this->rotation[2], this->rotation[0]);
        glm_normalize(this->rotation[0]);
        glm_normalize(this->rotation[1]);
    }*/
}

void Camera::rotate_all(vec3 angles) {
    Camera::rotate_camera(angles[0], X);
    Camera::rotate_camera(angles[1], Y);
    Camera::rotate_camera(angles[2], Z);
}

void Camera::rotate(
        enum coord_enum coord, float delta_time) {
    Camera::rotate_camera(-OBJECT_ROTATE_SPEED * delta_time, coord);
}

void Camera::get_animation(vec3 **animation) {
    *animation = &this->animation;
}

void Camera::set_animation(
        vec3 angles, float delta_time) {
    vec3 animation;
    glm_vec3_muladds(angles, delta_time, animation);
    glm_vec3_add(this->animation, animation,
                 this->animation);
}

void Camera::rotate_all_camera(vec3 angles) {
    Camera::rotate_all(angles);
}
