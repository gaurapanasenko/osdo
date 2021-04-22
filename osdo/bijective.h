#ifndef BIJECTIVE_H
#define BIJECTIVE_H

#include "osdo.h"

class Bijective {
public:
    virtual ~Bijective() {}
    virtual void get_position(vec4 **position) {}
    virtual void get_mat4(mat4 matrix) {}
    virtual void translate(vec3 distances, float delta_time) {}
    virtual void rotate(enum coord_enum coord, float delta_time) {}
    virtual void rotate_all(vec3 angles) {}
    virtual void get_animation(vec3 **animation) {}
    virtual void set_animation(vec3 angles, float delta_time) {}
};

#endif // BIJECTIVE_H
