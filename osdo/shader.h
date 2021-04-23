#ifndef SHADER_H
#define SHADER_H

#include "osdo.h"
#include "glbindable.h"
#include "EASTL/shared_ptr.h"
using eastl::shared_ptr;
using eastl::make_shared;

class Shader : public GlBindable {
    virtual void _bind(const GLuint id, GLenum target) const override;
public:
    Shader(const GLuint shader);
    ~Shader() override;

    static shared_ptr<Shader> create(const char *vertex_path,
                                     const char *fragment_path);

    void set_bool (const char* name, bool value);
    void set_int  (const char* name, int value);
    void set_float(const char* name, float value);
    void set_vec2 (const char* name, vec2 value);
    void set_vec2f(const char* name, float x, float y);
    void set_vec3 (const char* name, vec3 value);
    void set_vec3f(const char* name, float x, float y, float z);
    void set_vec4 (const char* name, vec4 value);
    void set_vec4f(const char* name, float x, float y, float z, float w);
    void set_mat2 (const char* name, mat2 mat);
    void set_mat3 (const char* name, mat3 mat);
    void set_mat4 (const char* name, mat4 mat);
};

#endif // SHADER_H
