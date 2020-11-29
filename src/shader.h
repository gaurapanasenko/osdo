#ifndef SHADER_H
#define SHADER_H

#include "osdo.h"

typedef struct Shader {
    char name[64];
    GLuint shader;
    UT_hash_handle hh;
} Shader;

bool shader_compile(const char* vertexCode, const char* fragmentCode,
                    Shader *shader);

// constructor generates the shader on the fly
bool shader_init(const char* name, Shader *shader);

void shader_del(Shader *shader);

// activate the shader
void shader_use(Shader *shader);

// utility uniform functions
void shader_set_bool (Shader *shader, const char* name, bool value);
void shader_set_int  (Shader *shader, const char* name, int value);
void shader_set_float(Shader *shader, const char* name, float value);
void shader_set_vec2 (Shader *shader, const char* name, vec2 value);
void shader_set_vec2f(Shader *shader, const char* name,
                      float x, float y);
void shader_set_vec3 (Shader *shader, const char* name, vec3 value);
void shader_set_vec3f(Shader *shader, const char* name,
                      float x, float y, float z);
void shader_set_vec4 (Shader *shader, const char* name, vec4 value);
void shader_set_vec4f(Shader *shader, const char* name,
                      float x, float y, float z, float w);
void shader_set_mat2 (Shader *shader, const char* name, mat2 mat);
void shader_set_mat3 (Shader *shader, const char* name, mat3 mat);
void shader_set_mat4 (Shader *shader, const char* name, mat4 mat);

#endif // SHADER_H
