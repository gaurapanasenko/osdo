#ifndef CAMERA_H
#define CAMERA_H

#include "osdo.h"

#include "bijective.h"

#define CAMERA_DIRECTION_INIT {0.0f, 0.0f, -1.0f, 0.0f}
#define CAMERA_DIRECTION (vec4 CAMERA_DIRECTION_INIT)

class Camera : public Bijective {
    mat4 rotation;
    vec4 position;
    vec4 animation;
public:
    Camera();

    void get_position(vec4 position) override;
    void set_position(vec4 position) override;

    void get_rotation(vec3 rotation) override;
    void set_rotation(vec3 rotation) override;

    void get_animation(vec3 animation) override;
    void set_animation(vec3 animation) override;

    void get_mat4(mat4 matrix) override;

    void translate(vec3 distances, float delta_time) override;
    void rotate(enum coord_enum coord, float delta_time) override;
    void rotate_all(vec3 angles) override;
    void add_animation(vec3 angles, float delta_time) override;

    void get_direction(vec4 dest);
    void get_rotation_mat4(mat4 dest);
    void get_rotation_inv_mat4(mat4 dest);

    void translate_camera(vec3 distances);

    void rotate_camera(float angle, enum coord_enum coord);
    void rotate_all_camera(vec3 angles);

};

#endif // CAMERA_H
