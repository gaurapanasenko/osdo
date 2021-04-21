#include <stdio.h>

#include "object.h"
#include "conf.h"
#include "shader.h"

Object::Object(shared_ptr<Model> model, shared_ptr<Shader> shader)
    : transform GLM_MAT4_IDENTITY_INIT,
      position GLM_VEC4_BLACK_INIT,
      animation GLM_VEC3_ZERO_INIT,
      model(model), shader(shader)
{
}


void Object::draw(mat4 mat4buf, GLdouble delta_time) {
    // render the loaded model
    Object::animate(static_cast<GLfloat>(delta_time));
    Object::get_mat4(mat4buf);
    this->shader->set_mat4("model", mat4buf);
    this->model->draw();
}

void Object::get_position(vec4 **position) {
    *position = &this->position;
}

void Object::get_mat4(mat4 dest) {
    glm_translate_make(dest, this->position);
    glm_mat4_mul(dest, this->transform, dest);
}

void Object::translate_object(vec3 distances) {
    glm_vec3_add(this->position, distances, this->position);
}

void Object::translate(vec3 distances, float delta_time) {
    vec3 new_distances = GLM_VEC3_ZERO_INIT;
    glm_vec3_muladds(distances, OBJECT_MOVE_SPEED * delta_time,
                     new_distances);
    Object::translate_object( new_distances);
}

void Object::rotate_object(float angle, enum coord_enum coord) {
    mat4 matrix = GLM_MAT4_IDENTITY_INIT;
    switch (coord) {
    case X: glm_rotate_x(matrix, angle, matrix); break;
    case Y: glm_rotate_y(matrix, angle, matrix); break;
    case Z: glm_rotate_z(matrix, angle, matrix); break;
    }
    glm_mat4_mul(matrix, this->transform, this->transform);
}

void Object::rotate_all_object(vec3 angles) {
    Object::rotate_object(angles[0], X);
    Object::rotate_object(angles[1], Y);
    Object::rotate_object(angles[2], Z);
}

void Object::rotate(enum coord_enum coord, float delta_time) {
    Object::rotate_object(delta_time * OBJECT_ROTATE_SPEED, coord);
}

void Object::rotate_all(vec3 angles) {
    Object::rotate_all_object(angles);
}

void Object::animate(float step) {
    vec3 animation = GLM_VEC3_ZERO_INIT;
    glm_vec3_muladds(this->animation, step, animation);
    Object::rotate_all(animation);
}

void Object::get_animation(vec3 **animation) {
    *animation = &this->animation;
}

void Object::set_animation(vec3 angles, float delta_time) {
    vec3 animation = GLM_VEC3_ZERO_INIT;
    glm_vec3_muladds(angles, delta_time, animation);
    glm_vec3_add(this->animation, animation,
                 this->animation);
}

void Object::scale(vec3 scale) {
    glm_scale(this->transform, scale);
}


mat4 *Object::get_transform()
{
    return &this->transform;
}
