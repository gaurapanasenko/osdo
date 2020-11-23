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
    GLint err_log_size = 0;
    GLchar *log;

    if (type == 0)
        glGetProgramiv(shader, GL_LINK_STATUS, &err_log_size);
    else
        glGetShaderiv(shader, GL_COMPILE_STATUS, &err_log_size);

    if (!err_log_size) {
        log = (GLchar*)malloc((unsigned long)err_log_size);
        if (log == NULL) {
            printf("Got some error, but cant allocate memory to read it.");
        }
        glGetShaderInfoLog(shader, err_log_size, NULL, log);
        printf("%s", log);
        free(log);
    }
    return (bool)err_log_size;
}

bool shader_compile(const char* vertexCode, const char* fragmentCode,
                    GLuint *program) {
    // 2. compile shaders
    GLuint vertex, fragment;

    // vertex shader
    vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &vertexCode, NULL);
    glCompileShader(vertex);
    if (!check_shader(vertex, 1)) {
        printf("Failed to compile vertex shader.\n%s", vertexCode);
        return false;
    }

    // fragment Shader
    fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &fragmentCode, NULL);
    glCompileShader(fragment);
    if (!check_shader(fragment, 1)) {
        printf("Failed to compile fragment shader.\n%s", fragmentCode);
        glDeleteShader(vertex);
        return false;
    }

    // shader Program
    *program = glCreateProgram();
    glAttachShader(*program, vertex);
    glAttachShader(*program, fragment);
    glLinkProgram(*program);
    if (!check_shader(*program, 0)) {
        printf("Failed to attach shaders.");
        glDeleteShader(vertex);
        glDeleteShader(fragment);
        return false;
    }

    // delete the shaders as they're linked into our program now and no longer necessery
    glDeleteShader(vertex);
    glDeleteShader(fragment);
    return true;
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

    if (!shader_compile(vertex, fragment, shader))
        return false;

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
