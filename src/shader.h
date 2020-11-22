#ifndef SHADER_H
#define SHADER_H

#include <cglm/cglm.h>

#include <GL/gl.h>

// constructor generates the shader on the fly
bool shader_init(const char* vertexPath, const char* fragmentPath,
                 GLuint *shader);

// activate the shader
void shader_use(const GLuint shader);

// utility uniform functions
void shader_set_bool (const GLuint shader, const char* name,
                      bool value);
void shader_set_int  (const GLuint shader, const char* name,
                      int value);
void shader_set_float(const GLuint shader, const char* name,
                      float value);
void shader_set_vec2 (const GLuint shader, const char* name,
                      const vec2 value);
void shader_set_vec2f(const GLuint shader, const char* name,
                      float x, float y);
void shader_set_vec3 (const GLuint shader, const char* name,
                      const vec3 value);
void shader_set_vec3f(const GLuint shader, const char* name,
                      float x, float y, float z);
void shader_set_vec4 (const GLuint shader, const char* name,
                      const vec4 value);
void shader_set_vec4f(const GLuint shader, const char* name,
                      float x, float y, float z, float w);
void shader_set_mat2 (const GLuint shader, const char* name,
                      const mat2 mat);
void shader_set_mat3 (const GLuint shader, const char* name,
                      const mat3 mat);
void shader_set_mat4 (const GLuint shader, const char* name,
                      const mat4 mat);

#endif // SHADER_H
