#ifndef SHADER_H
#define SHADER_H

#include <glm/glm.hpp>

#include <GL/gl.h>

class Shader {
public:
    unsigned int id;

    // constructor generates the shader on the fly
    Shader(const char* vertexPath, const char* fragmentPath);

    // activate the shader
    void use() const;

    // utility uniform functions
    void setBool(const char* name, bool value) const;
    void setInt(const char* name, int value) const;
    void setFloat(const char* name, float value) const;
    void setVec2(const char* name, const glm::vec2 &value) const;
    void setVec2(const char* name, float x, float y) const;
    void setVec3(const char* name, const glm::vec3 &value) const;
    void setVec3(const char* name, float x, float y, float z) const;
    void setVec4(const char* name, const glm::vec4 &value) const;
    void setVec4(const char* name, float x, float y, float z, float w) const;
    void setMat2(const char* name, const glm::mat2 &mat) const;
    void setMat3(const char* name, const glm::mat3 &mat) const;
    void setMat4(const char* name, const glm::mat4 &mat) const;

private:
    // utility function for checking shader compilation/linking errors.
    void checkCompileErrors(GLuint shader, const char* type);
};

#endif //SHADER_H
