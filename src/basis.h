#ifndef BASIS_H
#define BASIS_H

#include <cglm/cglm.h>
#include <cglm/quat.h>

#define CAMERA_DIRECTION_INIT {0.0f, 0.0f, -1.0f, 0.0f}
#define CAMERA_DIRECTION ((vec4)CAMERA_DIRECTION_INIT)

// Default camera values
static const float BASIS_MOVE_SPEED   =  0.5f;
static const float BASIS_ROTATE_SPEED =  0.5f;
static const float BASIS_ANIMATE_SPEED = 1.0f;

typedef struct Basis {
    mat4 view;
    vec3 animation;
    versor rotation;
    bool camera;
} Basis;

Basis basis_init(bool camera);
void basis_translate(Basis* basis, vec3 distances);
void basis_rotate(Basis* basis, float angle, vec3 axis);
void basis_rotate_all(Basis* basis, vec3 angles);

void basis_get_camera_position(Basis* basis, vec4 dest);
void basis_get_camera_direction(Basis* basis, vec4 dest);

void basis_get_position(Basis* basis, vec4 dest);

void basis_animate(Basis* basis, float deltaTime);


#endif // BASIS_H
