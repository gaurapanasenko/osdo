#include "bijective.h"

void bijective_get_position(Bijective bijective, vec4 **position) {
    bijective.type->get_position(
                bijective.bijective, position);
}

void bijective_get_mat4(Bijective bijective, mat4 matrix) {
    bijective.type->get_mat4(
                bijective.bijective, matrix);
}

void bijective_translate(Bijective bijective, vec3 distances, float delta_time) {
    bijective.type->translate(
                bijective.bijective, distances, delta_time);
}

void bijective_rotate(
        Bijective bijective, enum coord_enum coord, float delta_time) {
    bijective.type->rotate(
                bijective.bijective, coord, delta_time);
}

void bijective_rotate_all(Bijective bijective, vec3 angles) {
    bijective.type->rotate_all(
                bijective.bijective, angles);
}

void bijective_get_animation(Bijective bijective, vec3 **animation) {
    bijective.type->get_animation(
                bijective.bijective, animation);
}

void bijective_set_animation(Bijective bijective, vec3 angles, float delta_time) {
    bijective.type->set_animation(
                bijective.bijective, angles, delta_time);
}
