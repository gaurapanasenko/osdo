#include <stdio.h>
#include <cstring>

#include "shader.h"
#include "conf.h"
#include "EASTL/hash_map.h"
using eastl::hash_map;

static hash_map<ShaderType, GLenum> TYPES_MAP = {
    {VERT_SHADER, GL_VERTEX_SHADER},
    {TESC_SHADER, GL_TESS_CONTROL_SHADER},
    {TESE_SHADER, GL_TESS_EVALUATION_SHADER},
    {GEOM_SHADER, GL_GEOMETRY_SHADER},
    {FRAG_SHADER, GL_FRAGMENT_SHADER},
};

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

class ShaderSource {
    const GLuint id;
public:
    ShaderSource(const GLuint id) : id(id) {}
    static shared_ptr<ShaderSource> create(GLenum type, const char *code) {
        const GLuint shader = glCreateShader(type);
        glShaderSource(shader, 1, &code, nullptr);
        glCompileShader(shader);
        if (!check_shader(shader, 1)) {
            return {};
        }
        return make_shared<ShaderSource>(shader);
    }
    static shared_ptr<ShaderSource> create_file(GLenum type, const string& path) {
        GLchar* code = readFromFile(path.c_str());
        if (!code)
            return {};
        return create(type, code);
    }
    GLuint get_id() {return id;}
    void attach(const GLuint program) {
        glAttachShader(program, id);
    }
};

shared_ptr<Shader> compile(vector<shared_ptr<ShaderSource>> shaders) {
    for (auto &i : shaders) {
        if (!i)
            return {};
    }

    GLuint sh = glCreateProgram();
    for (auto &i : shaders) {
        i->attach(sh);
    }
    glLinkProgram(sh);
    if (!check_shader(sh, 0)) {
        return {};
    }

    return make_shared<Shader>(sh);
}

void Shader::_bind(const GLuint id, UNUSED GLenum target) const
{
    glUseProgram(id);
}

Shader::Shader(const GLuint shader) : GlBindable(shader) {}

Shader::~Shader() {
    glDeleteProgram(this->get_id());
}

shared_ptr<Shader> Shader::create(const Shader::shader_map& shaders_paths)
{
    vector<shared_ptr<ShaderSource>> shaders;
    for (auto& i : shaders_paths) {
        shaders.push_back(
                    ShaderSource::create_file(TYPES_MAP[i.first], i.second));
    }
    return compile(shaders);
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
