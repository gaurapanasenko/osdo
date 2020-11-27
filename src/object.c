#include <stdio.h>

#include "object.h"
#include "conf.h"
#include "app.h"
#include "shader.h"

Object object_init(Mesh *mesh, Shader *shader) {
    return OBJECT(mesh, shader);
}

void object_init_empty(void *object) {
    *((Object*)object) = OBJECT_EMPTY;
}

void object_draw(Object *object, mat4 mat4buf, GLdouble delta_time) {
    // render the loaded model
    Mesh *mesh = object->mesh;
    object_animate(object, (GLfloat)delta_time);
    glm_translate_make(mat4buf, object->position);
    glm_mat4_mul(mat4buf, object->transform, mat4buf);
    shader_set_mat4(object->shader, "model", mat4buf);
    if (mesh)
        mesh_draw(mesh);
    else
        printf("Failed to find mesh.\n");
}

void object_translate(Object* object, vec3 distances) {
    glm_vec3_add(object->position, distances, object->position);
}

void object_translate_transformable(
        void* object, vec3 distances, float delta_time) {
    vec3 new_distances = GLM_VEC3_ZERO_INIT;
    glm_vec3_muladds(distances, OBJECT_MOVE_SPEED * delta_time,
                     new_distances);
    object_translate((Object*)object, new_distances);
}

void object_rotate(Object* object, float angle, vec3 axis) {
    mat4 matrix;
    glm_rotate_make(matrix, angle, axis);
    glm_mat4_mul(matrix, object->transform, object->transform);
}

void object_rotate_all(Object *object, vec3 angles) {
    object_rotate(object, angles[0], GLM_XUP);
    object_rotate(object, angles[1], GLM_YUP);
    object_rotate(object, angles[2], GLM_ZUP);
}

void object_rotate_transformable(
        void* object, vec3 axis, float delta_time) {
    object_rotate((Object*)object, delta_time * OBJECT_ROTATE_SPEED,
                  axis);
}

void object_animate(Object* object, float step) {
    vec3 animation = GLM_VEC3_ZERO_INIT;
    glm_vec3_muladds(object->animation, step, animation);
    object_rotate_all(object, animation);
}

void object_set_animation_transformable(
        void* object, vec3 angles, float delta_time) {
    vec3 animation = GLM_VEC3_ZERO_INIT;
    glm_vec3_muladds(angles, delta_time, animation);
    glm_vec3_add(((Object*)object)->animation, animation,
                 ((Object*)object)->animation);
}
