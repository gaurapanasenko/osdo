#include <stdio.h>

#include "basis.h"
#include "conf.h"

Basis basis_init(bool camera) {
    Basis basis = {
        GLM_MAT4_IDENTITY_INIT,
        GLM_VEC3_ZERO_INIT,
        GLM_QUAT_IDENTITY_INIT,
        camera
    };
    return basis;
}

void basis_translate(Basis* basis, vec3 distances) {
    mat4 matrix = GLM_MAT4_IDENTITY_INIT;
    glm_translate(matrix, distances);
    glm_mat4_mul(matrix, basis->view, basis->view);
}

void basis_translate_speed(Basis* basis, vec3 distances) {
    vec3 new_distances;
    glm_vec3_muladds(distances, BASIS_MOVE_SPEED, new_distances);
    basis_translate(basis, new_distances);
}

void basis_rotate(Basis* basis, float angle, vec3 axis) {
    float new_angle = BASIS_MOVE_SPEED * angle;
    if (basis->camera) {
        mat4 matrix = GLM_MAT4_IDENTITY_INIT;
        glm_rotate(matrix, new_angle, axis);
        glm_mat4_mul(matrix, basis->view, basis->view);
    } else {
        versor qantnerion;
        glm_quatv(qantnerion, new_angle, axis);
        glm_quat_mul(qantnerion, basis->rotation, basis->rotation);
    }
}

void basis_rotate_speed(Basis* basis, float angle, vec3 axis) {
    basis_rotate(basis, BASIS_ROTATE_SPEED * angle, axis);
}

void basis_rotate_all(Basis *basis, vec3 angles) {
    basis_rotate(basis, angles[0], GLM_XUP);
    basis_rotate(basis, angles[1], GLM_YUP);
    basis_rotate(basis, angles[2], GLM_ZUP);
}

void basis_get_camera_position(Basis* basis, vec4 dest) {
    mat4 view = GLM_MAT4_IDENTITY_INIT;
    glm_vec4_copy(GLM_VEC4_BLACK, dest);
    glm_mat4_inv(basis->view, view);
    glm_mat4_mulv(view, dest, dest);
}

void basis_get_camera_direction(Basis* basis, vec4 dest) {
    glm_vec4_copy(CAMERA_DIRECTION, dest);
    if (!basis->camera) {
        glm_mat4_mulv(basis->view, dest, dest);
    } else {
        mat4 view, mat4 = GLM_MAT4_IDENTITY_INIT;
        glm_quat_rotate(mat4, basis->rotation, view);
        glm_mat4_mulv(view, dest, dest);
    }
}


void basis_get_position(Basis* basis, vec4 dest) {
    vec4 black = GLM_VEC4_BLACK_INIT;
    glm_vec4_copy(black, dest);
    glm_mat4_mulv(basis->view, dest, dest);
}

void basis_animate(Basis* basis, float deltaTime) {
    vec3 animation;
    glm_vec3_muladds(basis->animation, BASIS_ANIMATE_SPEED * deltaTime,
                     animation);
    basis_rotate_all(basis, animation);
}
