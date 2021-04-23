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
    vec4 animation;
    shared_ptr<Model> model;

public:
    Object(shared_ptr<Model> model = nullptr);
    ~Object() override = default;

    void get_position(vec4 position) override;
    void set_position(vec4 position) override;

    void get_rotation(vec3 rotation) override;
    void set_rotation(vec3 rotation) override;

    void get_animation(vec3 rotation) override;
    void set_animation(vec3 rotation) override;

    void get_mat4(mat4 matrix) override;

    void translate(vec3 distances, float delta_time) override;
    void rotate(enum coord_enum coord, float delta_time) override;
    void rotate_all(vec3 angles) override;
    void add_animation(vec3 angles, float delta_time) override;

    shared_ptr<Model> get_model();

    void draw(Shader &shader, mat4 mat4buf, GLdouble delta_time);

    void translate_object(vec3 distances);

    void rotate_object(float angle, enum coord_enum coord);
    void rotate_all_object(vec3 angles);

    void animate(float step);

    void scale(vec3 scale);
    mat4* get_transform();
};

#endif // OBJECT_H
