#ifndef SHADER_H
#define SHADER_H

#include "osdo.h"
#include <memory>
using std::shared_ptr;
using std::make_shared;

class Shader {
    const GLuint shader;
public:
    Shader(const GLuint shader);
    ~Shader();

    Shader(const Shader&) = delete;
    Shader(Shader&&) = delete;

    Shader& operator=(const Shader&) = delete;
    Shader& operator=(Shader&&) = delete;

    static shared_ptr<Shader> create(const char* name);

    void use();

    void set_bool (const char* name, bool value);
    void set_int  (const char* name, int value);
    void set_float(const char* name, float value);
    void set_vec2 (const char* name, vec2 value);
    void set_vec2f(const char* name,
                          float x, float y);
    void set_vec3 (const char* name, vec3 value);
    void set_vec3f(const char* name,
                          float x, float y, float z);
    void set_vec4 (const char* name, vec4 value);
    void set_vec4f(const char* name,
                          float x, float y, float z, float w);
    void set_mat2 (const char* name, mat2 mat);
    void set_mat3 (const char* name, mat3 mat);
    void set_mat4 (const char* name, mat4 mat);
};

#endif // SHADER_H
