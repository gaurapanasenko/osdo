#include <stdio.h>

#include "object.h"
#include "conf.h"
#include "app.h"
#include "shader.h"

void object_init(Object *object, Model *model, Shader *shader) {
    *object = OBJECT(model, shader);
}

void object_init_empty(void *object) {
    *((Object*)object) = OBJECT_EMPTY;
}

void object_draw(Object *object, mat4 mat4buf, GLdouble delta_time) {
    // render the loaded model
    object_animate(object, (GLfloat)delta_time);
    object_get_mat4(object, mat4buf);
    shader_set_mat4(object->shader, "model", mat4buf);
    model_draw(object->model);
}

void object_get_position_bijective(Object *object, vec4 **position) {
    *position = &object->position;
}

void object_get_mat4(Object *object, mat4 dest) {
    glm_translate_make(dest, object->position);
    glm_mat4_mul(dest, object->transform, dest);
}

void object_translate(Object* object, vec3 distances) {
    glm_vec3_add(object->position, distances, object->position);
}

void object_translate_bijective(Object *object, vec3 distances, float delta_time) {
    vec3 new_distances = GLM_VEC3_ZERO_INIT;
    glm_vec3_muladds(distances, OBJECT_MOVE_SPEED * delta_time,
                     new_distances);
    object_translate(object, new_distances);
}

void object_rotate(Object* object, float angle, enum coord_enum coord) {
    mat4 matrix = GLM_MAT4_IDENTITY_INIT;
    switch (coord) {
    case X: glm_rotate_x(matrix, angle, matrix); break;
    case Y: glm_rotate_y(matrix, angle, matrix); break;
    case Z: glm_rotate_z(matrix, angle, matrix); break;
    }
    glm_mat4_mul(matrix, object->transform, object->transform);
}

void object_rotate_all(Object *object, vec3 angles) {
    object_rotate(object, angles[0], X);
    object_rotate(object, angles[1], Y);
    object_rotate(object, angles[2], Z);
}

void object_rotate_bijective(Object *object, enum coord_enum coord,
                             float delta_time) {
    object_rotate(object, delta_time * OBJECT_ROTATE_SPEED, coord);
}

void object_rotate_all_bijective(Object *object, vec3 angles) {
    object_rotate_all(object, angles);
}

void object_animate(Object* object, float step) {
    vec3 animation = GLM_VEC3_ZERO_INIT;
    glm_vec3_muladds(object->animation, step, animation);
    object_rotate_all(object, animation);
}

void object_get_animation(Object* object, vec3 **animation) {
    *animation = &object->animation;
}

void object_set_animation(Object* object, vec3 angles, float delta_time) {
    vec3 animation = GLM_VEC3_ZERO_INIT;
    glm_vec3_muladds(angles, delta_time, animation);
    glm_vec3_add(object->animation, animation,
                 object->animation);
}

void object_scale(Object *object, vec3 scale) {
    glm_scale(object->transform, scale);
}
