#include <stdio.h>

#include "object.h"
#include "conf.h"
#include "app.h"
#include "shader.h"

Object object_init(Mesh *mesh, GLuint *shader) {
    Object object = {
        GLM_MAT4_IDENTITY_INIT,
        GLM_VEC4_BLACK_INIT,
        GLM_VEC3_ZERO_INIT, mesh, shader,
        {
            object_translate_transformable,
            object_rotate_transformable,
            object_animate_transformable
        }
    };
    return object;
}

void object_draw(App *app, Window *win, Object *object) {
    // render the loaded model
    GlMesh *mesh;
    object_animate(object, (GLfloat)app->delta_time);
    glm_mat4_copy(GLM_MAT4_IDENTITY, win->mat4buf);
    glm_translate(win->mat4buf, object->position);
    glm_mat4_mul(win->mat4buf, object->transform, win->mat4buf);
    shader_set_mat4(*object->shader, "model", win->mat4buf);
    HASH_FIND_INT(win->meshes, &object->mesh, mesh);
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
    mat4 matrix = GLM_MAT4_IDENTITY_INIT;
    glm_rotate(matrix, angle, axis);
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

void object_animate_transformable(
        void* object, vec3 angles, float delta_time) {
    vec3 animation = GLM_VEC3_ZERO_INIT;
    glm_vec3_muladds(angles, delta_time, animation);
    glm_vec3_add(((Object*)object)->animation, animation,
                 ((Object*)object)->animation);
}
