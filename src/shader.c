#include <stdio.h>
#include <string.h>
#include <GL/glew.h>

#include "shader.h"

char * readFromFile(const char *path) {
    char* data;
    size_t size;
    FILE *file = fopen(path, "r");
    if (file == NULL) {
        printf("ERROR: failed to open file %s\n", path);
        return NULL;
    }
    fseek(file, 0L, SEEK_END);
    size = (size_t)ftell(file);
    fseek(file, 0L, SEEK_SET);
    data = (char*) malloc(size + 1);
    fread(data, 1, size, file);
    data[size] = 0;
    fclose(file);
    return data;
}

// utility function for checking shader compilation/linking errors.
bool check_shader(GLuint shader, const int type) {
    GLint success = -1;
    GLchar infoLog[1024];

    if (type == 0)
        glGetProgramiv(shader, GL_LINK_STATUS, &success);
    else
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

    if (!success) {
        glGetShaderInfoLog(shader, 1024, NULL, infoLog);
        printf("%s", infoLog);
    }
    return (bool)success;
}

GLuint compile_shaders(const char* vertexCode, const char* fragmentCode) {
    // 2. compile shaders
    GLuint id = 0, vertex, fragment;

    // vertex shader
    vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &vertexCode, NULL);
    glCompileShader(vertex);
    if (!check_shader(vertex, 1)) {
        printf("Failed to compile vertex shader.\n%s", vertexCode);
        return 0;
    }

    // fragment Shader
    fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &fragmentCode, NULL);
    glCompileShader(fragment);
    if (!check_shader(fragment, 1)) {
        printf("Failed to compile fragment shader.\n%s", fragmentCode);
        glDeleteShader(vertex);
        return 0;
    }

    // shader Program
    id = glCreateProgram();
    glAttachShader(id, vertex);
    glAttachShader(id, fragment);
    glLinkProgram(id);
    if (!check_shader(id, 0)) {
        printf("Failed to attach shaders.");
        glDeleteShader(vertex);
        glDeleteShader(fragment);
        return 0;
    }

    // delete the shaders as they're linked into our program now and no longer necessery
    glDeleteShader(vertex);
    glDeleteShader(fragment);
    return id;
}

bool shader_init(const char* vertexPath, const char* fragmentPath,
                 GLuint *shader) {
    // 1. retrieve the vertex/fragment source code from filePath
    GLchar* vertex = readFromFile(vertexPath);
    if (vertex == NULL) {
        printf("ERROR: failed to read from vertex shader file %s.\n",
               vertexPath);
        return false;
    }

    GLchar* fragment = readFromFile(fragmentPath);
    if (fragment == NULL) {
        printf("ERROR: failed to read from fragment shader file %s.\n",
               fragmentPath);
        free(vertex);
        return false;
    }

    *shader = compile_shaders(vertex, fragment);

    free(vertex);
    free(fragment);

    return true;
}

void shader_use(const GLuint shader) {
    glUseProgram(shader);
}

void shader_set_bool(const GLuint shader, const char* name, bool value) {
    glUniform1i(glGetUniformLocation(shader, name), (int)value);
}

void shader_set_int(const GLuint shader, const char* name, int value) {
    glUniform1i(glGetUniformLocation(shader, name), value);
}

void shader_set_float(const GLuint shader, const char* name, float value) {
    glUniform1f(glGetUniformLocation(shader, name), value);
}

void shader_set_vec2(const GLuint shader, const char* name, const vec2 value) {
    glUniform2fv(glGetUniformLocation(shader, name), 1, &value[0]);
}

void shader_set_vec2f(const GLuint shader, const char* name, float x, float y) {
    glUniform2f(glGetUniformLocation(shader, name), x, y);
}

void shader_set_vec3(const GLuint shader, const char* name, const vec3 value) {
    glUniform3fv(glGetUniformLocation(shader, name), 1, &value[0]);
}

void shader_set_vec3f(const GLuint shader, const char* name, float x, float y, float z) {
    glUniform3f(glGetUniformLocation(shader, name), x, y, z);
}

void shader_set_vec4(const GLuint shader, const char* name, const vec4 value) {
    glUniform4fv(glGetUniformLocation(shader, name), 1, &value[0]);
}

void shader_set_vec4f(const GLuint shader, const char* name, float x, float y, float z, float w) {
    glUniform4f(glGetUniformLocation(shader, name), x, y, z, w);
}

void shader_set_mat2(const GLuint shader, const char* name, const mat2 mat) {
    glUniformMatrix2fv(glGetUniformLocation(shader, name), 1, GL_FALSE, &mat[0][0]);
}

void shader_set_mat3(const GLuint shader, const char* name, const mat3 mat) {
    glUniformMatrix3fv(glGetUniformLocation(shader, name), 1, GL_FALSE, &mat[0][0]);
}

void shader_set_mat4(const GLuint shader, const char* name, const mat4 mat) {
    glUniformMatrix4fv(glGetUniformLocation(shader, name), 1, GL_FALSE, &mat[0][0]);
}
