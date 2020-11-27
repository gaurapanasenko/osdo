#include <stdio.h>
#include <string.h>
#include <GL/glew.h>

#include "shader.h"
#include "conf.h"

#define VERTEX_PATH RES_DIR"/%s.vs"
#define FRAGMENT_PATH RES_DIR"/%s.fs"

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
                    Shader *shader) {
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
    shader->shader = glCreateProgram();
    glAttachShader(shader->shader, vertex);
    glAttachShader(shader->shader, fragment);
    glLinkProgram(shader->shader);
    if (!check_shader(shader->shader, 0)) {
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

bool shader_init(const char* name, Shader *shader) {
    // 1. retrieve the vertex/fragment source code from filePath
    const size_t path_len = strlen(VERTEX_PATH);
    const size_t len = strlen(name);
    strcpy(shader->name, name);
    char *vertex_path = calloc(len + path_len, sizeof(char)),
        *fragment_path = calloc(len + path_len, sizeof(char));
    snprintf(vertex_path, len + path_len, VERTEX_PATH, name);
    snprintf(fragment_path, len + path_len, FRAGMENT_PATH, name);
    GLchar* vertex = readFromFile(vertex_path);
    if (vertex == NULL) {
        printf("ERROR: failed to read from vertex shader file %s.\n",
               vertex_path);
        free(vertex_path);
        free(fragment_path);
        return false;
    }

    GLchar* fragment = readFromFile(fragment_path);
    if (fragment == NULL) {
        printf("ERROR: failed to read from fragment shader file %s.\n",
               fragment_path);
        free(vertex_path);
        free(fragment_path);
        free(vertex);
        return false;
    }

    if (!shader_compile(vertex, fragment, shader))
        return false;

    free(vertex);
    free(fragment);
    free(vertex_path);
    free(fragment_path);

    return true;
}

void shader_del(Shader *shader) {
    glDeleteProgram(shader->shader);
}

void shader_use(Shader *shader) {
    glUseProgram(shader->shader);
}

void shader_set_bool(Shader *shader, const char* name, bool value) {
    glUniform1i(glGetUniformLocation(shader->shader, name), (int)value);
}

void shader_set_int(Shader *shader, const char* name, int value) {
    glUniform1i(glGetUniformLocation(shader->shader, name), value);
}

void shader_set_float(Shader *shader, const char* name, float value) {
    glUniform1f(glGetUniformLocation(shader->shader, name), value);
}

void shader_set_vec2(Shader *shader, const char* name, vec2 value) {
    glUniform2fv(glGetUniformLocation(shader->shader, name),
                 1, &value[0]);
}

void shader_set_vec2f(Shader *shader, const char* name,
                      float x, float y) {
    glUniform2f(glGetUniformLocation(shader->shader, name), x, y);
}

void shader_set_vec3(Shader *shader, const char* name, vec3 value) {
    glUniform3fv(glGetUniformLocation(shader->shader, name),
                 1, &value[0]);
}

void shader_set_vec3f(Shader *shader, const char* name,
                      float x, float y, float z) {
    glUniform3f(glGetUniformLocation(shader->shader, name), x, y, z);
}

void shader_set_vec4(Shader *shader, const char* name, vec4 value) {
    glUniform4fv(glGetUniformLocation(shader->shader, name),
                 1, &value[0]);
}

void shader_set_vec4f(Shader *shader, const char* name,
                      float x, float y, float z, float w) {
    glUniform4f(glGetUniformLocation(shader->shader, name), x, y, z, w);
}

void shader_set_mat2(Shader *shader, const char* name, mat2 mat) {
    glUniformMatrix2fv(glGetUniformLocation(shader->shader, name),
                       1, GL_FALSE, &mat[0][0]);
}

void shader_set_mat3(Shader *shader, const char* name, mat3 mat) {
    glUniformMatrix3fv(glGetUniformLocation(shader->shader, name),
                       1, GL_FALSE, &mat[0][0]);
}

void shader_set_mat4(Shader *shader, const char* name, mat4 mat) {
    glUniformMatrix4fv(glGetUniformLocation(shader->shader, name),
                       1, GL_FALSE, &mat[0][0]);
}
