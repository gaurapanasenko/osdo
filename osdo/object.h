#ifndef OBJECT_H
#define OBJECT_H

#include "osdo.h"

#include "bijective.h"
#include "model.h"
#include "shader.h"
#include "EASTL/shared_ptr.h"
using eastl::shared_ptr;
using eastl::make_shared;

class Object : public Bijective {
    mat4 transform;
    vec4 position;
    vec3 animation;
    shared_ptr<Model> model;
    shared_ptr<Shader> shader;

public:
    Object(shared_ptr<Model> model = nullptr, shared_ptr<Shader> shader = nullptr);
    ~Object() override = default;

    void draw(mat4 mat4buf, GLdouble delta_time);

    void get_position(vec4 **position) override;
    void get_mat4(mat4 dest) override;

    void translate_object(vec3 distances);
    void translate(vec3 distances, float delta_time) override;

    void rotate_object(float angle, enum coord_enum coord);
    void rotate_all_object(vec3 angles);
    void rotate(enum coord_enum coord, float delta_time) override;
    void rotate_all(vec3 angles) override;

    void animate(float step);
    void get_animation(vec3 **animation) override;
    void set_animation(vec3 angles, float delta_time) override;

    void scale(vec3 scale);
    mat4* get_transform();
};

#endif // OBJECT_H
