#include <stdio.h>

#include "object.h"
#include "conf.h"
#include "app.h"
#include "shader.h"

Object object_init(Mesh *mesh, GLuint *shader) {
    Object object = {
        GLM_MAT4_IDENTITY_INIT,
        GLM_VEC3_ZERO_INIT,
        GLM_VEC3_ZERO_INIT, mesh, shader
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

void object_translate_camera(Object* object, vec3 distances) {
    vec4 dest = GLM_VEC4_BLACK_INIT, dist = GLM_VEC4_BLACK_INIT;
    glm_vec3_copy(distances, dist);
    glm_mat4_mulv(object->transform, dist, dest);
    object_translate(object, dest);
}

void object_translate_speed(Object* object, vec3 distances, int active,
                            float s) {
    vec3 new_distances = GLM_VEC3_ZERO_INIT;
    glm_vec3_muladds(distances, OBJECT_MOVE_SPEED * s, new_distances);
    if (active)
        object_translate(object, new_distances);
    else
        object_translate_camera(object, new_distances);
}

void object_rotate(Object* object, float angle, vec3 axis) {
    glm_rotate(object->transform, angle, axis);
}

void object_rotate_speed(Object* object, float angle, vec3 axis) {
    object_rotate(object, OBJECT_ROTATE_SPEED * angle, axis);
}

void object_rotate_all(Object *object, vec3 angles) {
    object_rotate(object, angles[0], GLM_XUP);
    object_rotate(object, angles[1], GLM_YUP);
    object_rotate(object, angles[2], GLM_ZUP);
}

void object_get_camera_direction(Object* object, vec4 dest) {
    glm_vec4_copy(CAMERA_DIRECTION, dest);
    glm_mat4_mulv(object->transform, dest, dest);
}
/*


void object_get_position(Object* object, vec4 dest) {
    vec4 black = GLM_VEC4_BLACK_INIT;
    glm_vec4_copy(black, dest);
    glm_mat4_mulv(object->view, dest, dest);
}
*/

void object_animate(Object* object, float deltaTime) {
    vec3 animation;
    glm_vec3_muladds(object->animation, OBJECT_ANIMATE_SPEED * deltaTime,
                     animation);
    object_rotate_all(object, animation);
}
