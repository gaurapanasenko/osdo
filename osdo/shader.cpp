#include <stdio.h>
#include <cstring>

#include "shader.h"
#include "conf.h"

#define VERTEX_PATH RES_DIR"/%s.vs"
#define FRAGMENT_PATH RES_DIR"/%s.fs"

char * readFromFile(const char *path) {
    char* data;
    size_t size;
    FILE *file = fopen(path, "r");
    if (file == nullptr) {
        printf("ERROR: failed to open file %s\n", path);
        return nullptr;
    }
    fseek(file, 0L, SEEK_END);
    size = static_cast<size_t>(ftell(file));
    fseek(file, 0L, SEEK_SET);
    data = static_cast<char*>(malloc(size + 1));
    fread(data, 1, size, file);
    data[size] = 0;
    fclose(file);
    return data;
}

// utility function for checking shader compilation/linking errors.
bool check_shader(GLuint shader, const int type) {
    GLint status = 0, size = 0;
    GLchar *log;
    GLuint status_type = GL_COMPILE_STATUS;
    void (*gl_get)(GLuint, GLuint, GLint*) = glGetShaderiv;

    if (type == 0) {
        gl_get = glGetProgramiv;
        status_type = GL_LINK_STATUS;
    }

    gl_get(shader, status_type, &status);
    if (status == GL_FALSE) {
        gl_get(shader, GL_INFO_LOG_LENGTH, &size);
        log = static_cast<GLchar*>(malloc(static_cast<size_t>(size)));
        if (log == nullptr) {
            printf("Got some error, but cant allocate memory to read it.\n");
            return false;
        }
        glGetShaderInfoLog(shader, size, &size, log);
        puts(log);
        fflush(stdout);
        free(log);
        return false;
    }
    return true;
}

bool compile(const char* vertexCode, const char* fragmentCode, GLuint *shader) {
    // 2. compile shaders
    GLuint vertex, fragment, sh;

    // vertex shader
    vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &vertexCode, nullptr);
    glCompileShader(vertex);
    if (!check_shader(vertex, 1)) {
        printf("Failed to compile vertex shader.\n%s", vertexCode);
        return false;
    }

    // fragment Shader
    fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &fragmentCode, nullptr);
    glCompileShader(fragment);
    if (!check_shader(fragment, 1)) {
        printf("Failed to compile fragment shader.\n%s", fragmentCode);
        glDeleteShader(vertex);
        return false;
    }

    // shader Program
    sh = glCreateProgram();
    glAttachShader(sh, vertex);
    glAttachShader(sh, fragment);
    glLinkProgram(sh);
    if (!check_shader(sh, 0)) {
        printf("Failed to attach shaders.");
        glDeleteShader(vertex);
        glDeleteShader(fragment);
        return false;
    }

    // delete the shaders as they're linked into our program now and no longer necessery
    glDeleteShader(vertex);
    glDeleteShader(fragment);
    *shader = sh;
    return true;
}

void Shader::_bind(const GLuint id, UNUSED GLenum target) const
{
    glUseProgram(id);
}

Shader::Shader(const GLuint shader) : GlBindable(shader) {}

Shader::~Shader() {
    glDeleteProgram(this->get_id());
}

shared_ptr<Shader> Shader::create(const char* name) {
    // 1. retrieve the vertex/fragment source code from filePath
    const size_t path_len = strlen(VERTEX_PATH);
    const size_t len = strlen(name);
    char *vertex_path = static_cast<char*>(calloc(len + path_len, sizeof(char))),
        *fragment_path = static_cast<char*>(calloc(len + path_len, sizeof(char)));
    snprintf(vertex_path, len + path_len, VERTEX_PATH, name);
    snprintf(fragment_path, len + path_len, FRAGMENT_PATH, name);
    GLchar* vertex = readFromFile(vertex_path);
    if (vertex == nullptr) {
        printf("ERROR: failed to read from vertex shader file %s.\n",
               vertex_path);
        free(vertex_path);
        free(fragment_path);
        return nullptr;
    }

    GLchar* fragment = readFromFile(fragment_path);
    if (fragment == nullptr) {
        printf("ERROR: failed to read from fragment shader file %s.\n",
               fragment_path);
        free(vertex_path);
        free(fragment_path);
        free(vertex);
        return nullptr;
    }

    GLuint shader;
    if (!compile(vertex, fragment, &shader))
        return nullptr;

    free(vertex);
    free(fragment);
    free(vertex_path);
    free(fragment_path);

    return make_shared<Shader>(shader);
}

void Shader::set_bool(const char* name, bool value) {
    glUniform1i(glGetUniformLocation(this->get_id(), name), static_cast<int>(value));
}

void Shader::set_int(const char* name, int value) {
    glUniform1i(glGetUniformLocation(this->get_id(), name), value);
}

void Shader::set_float(const char* name, float value) {
    glUniform1f(glGetUniformLocation(this->get_id(), name), value);
}

void Shader::set_vec2(const char* name, vec2 value) {
    glUniform2fv(glGetUniformLocation(this->get_id(), name),
                 1, &value[0]);
}

void Shader::set_vec2f(const char* name,
                      float x, float y) {
    glUniform2f(glGetUniformLocation(this->get_id(), name), x, y);
}

void Shader::set_vec3(const char* name, vec3 value) {
    glUniform3fv(glGetUniformLocation(this->get_id(), name),
                 1, &value[0]);
}

void Shader::set_vec3f(const char* name,
                      float x, float y, float z) {
    glUniform3f(glGetUniformLocation(this->get_id(), name), x, y, z);
}

void Shader::set_vec4(const char* name, vec4 value) {
    glUniform4fv(glGetUniformLocation(this->get_id(), name),
                 1, &value[0]);
}

void Shader::set_vec4f(const char* name,
                      float x, float y, float z, float w) {
    glUniform4f(glGetUniformLocation(this->get_id(), name), x, y, z, w);
}

void Shader::set_mat2(const char* name, mat2 mat) {
    glUniformMatrix2fv(glGetUniformLocation(this->get_id(), name),
                       1, GL_FALSE, &mat[0][0]);
}

void Shader::set_mat3(const char* name, mat3 mat) {
    glUniformMatrix3fv(glGetUniformLocation(this->get_id(), name),
                       1, GL_FALSE, &mat[0][0]);
}

void Shader::set_mat4(const char* name, mat4 mat) {
    glUniformMatrix4fv(glGetUniformLocation(this->get_id(), name),
                       1, GL_FALSE, &mat[0][0]);
}
