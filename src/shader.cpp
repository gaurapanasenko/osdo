#include <cstdio>
#include <cstring>

#include <glad/glad.h>

#include "shader.h"

char * readFromFile(const char *path) {
    char* data;
    int size;
    FILE *file = fopen(path, "r");
    if (file == NULL) {
        printf("ERROR: failed to open file %s", path);
        return NULL;
    }
    fseek(file, 0L, SEEK_END);
    size = ftell(file);
    data = (char*) calloc(size, sizeof(char));
    fgets(data, size, file);
    fclose(file);
    return data;
}

Shader::Shader(const char *vertexPath, const char *fragmentPath) : id(-1) {
    // 1. retrieve the vertex/fragment source code from filePath
    char* vertexCode = readFromFile(vertexPath);
    if (vertexCode == NULL) {
        printf("ERROR: failed to read from vertex shader file.");
    }

    char* fragmentCode = readFromFile(fragmentPath);
    if (vertexCode == NULL) {
        free(vertexCode);
        printf("ERROR: failed to read from vertex shader file.");
    }

    // 2. compile shaders
    unsigned int vertex, fragment;

    // vertex shader
    vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &vertexCode, NULL);
    glCompileShader(vertex);
    checkCompileErrors(vertex, "VERTEX");

    // fragment Shader
    fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &fragmentCode, NULL);
    glCompileShader(fragment);
    checkCompileErrors(fragment, "FRAGMENT");

    // shader Program
    id = glCreateProgram();
    glAttachShader(id, vertex);
    glAttachShader(id, fragment);
    glLinkProgram(id);
    checkCompileErrors(id, "PROGRAM");

    // delete the shaders as they're linked into our program now and no longer necessery
    glDeleteShader(vertex);
    glDeleteShader(fragment);

    free(vertexCode);
    free(fragmentCode);
}

void Shader::use() const {
    glUseProgram(id);
}

void Shader::setBool(const char* name, bool value) const {
    glUniform1i(glGetUniformLocation(id, name), (int)value);
}

void Shader::setInt(const char* name, int value) const {
    glUniform1i(glGetUniformLocation(id, name), value);
}

void Shader::setFloat(const char* name, float value) const {
    glUniform1f(glGetUniformLocation(id, name), value);
}

void Shader::setVec2(const char* name, const glm::vec2 &value) const {
    glUniform2fv(glGetUniformLocation(id, name), 1, &value[0]);
}

void Shader::setVec2(const char* name, float x, float y) const {
    glUniform2f(glGetUniformLocation(id, name), x, y);
}

void Shader::setVec3(const char* name, const glm::vec3 &value) const {
    glUniform3fv(glGetUniformLocation(id, name), 1, &value[0]);
}

void Shader::setVec3(const char* name, float x, float y, float z) const {
    glUniform3f(glGetUniformLocation(id, name), x, y, z);
}

void Shader::setVec4(const char* name, const glm::vec4 &value) const {
    glUniform4fv(glGetUniformLocation(id, name), 1, &value[0]);
}

void Shader::setVec4(const char* name, float x, float y, float z, float w) const {
    glUniform4f(glGetUniformLocation(id, name), x, y, z, w);
}

void Shader::setMat2(const char* name, const glm::mat2 &mat) const {
    glUniformMatrix2fv(glGetUniformLocation(id, name), 1, GL_FALSE, &mat[0][0]);
}

void Shader::setMat3(const char* name, const glm::mat3 &mat) const {
    glUniformMatrix3fv(glGetUniformLocation(id, name), 1, GL_FALSE, &mat[0][0]);
}

void Shader::setMat4(const char* name, const glm::mat4 &mat) const {
    glUniformMatrix4fv(glGetUniformLocation(id, name), 1, GL_FALSE, &mat[0][0]);
}

void Shader::checkCompileErrors(GLuint shader, const char* type) {
    GLint success;
    GLchar infoLog[1024];

    if (strcmp(type, "PROGRAM"))
        glGetProgramiv(shader, GL_LINK_STATUS, &success);
    else
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

    if (!success) {
        glGetShaderInfoLog(shader, 1024, NULL, infoLog);
        printf("ERROR: of type %s:%s\n", type, infoLog);
    }
}
