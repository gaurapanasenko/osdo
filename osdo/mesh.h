#ifndef MESH_H
#define MESH_H

#include "osdo.h"
#include "model.h"
#include "vertex.h"

class Mesh : public Model {
protected:
    GLuint vao, vbo, ebo;
    GLint indices_size;
public:
    Mesh();
    ~Mesh() override;

    Mesh(const Mesh&) = delete;
    Mesh(Mesh&&) = delete;
    Mesh& operator=(const Mesh&) = delete;
    Mesh& operator=(Mesh&&) = delete;

    void cube_update();
    void update(const Vertex* vertices, size_t vertices_n,
                const GLuint* indices, size_t indices_n);

    void draw(Shader &shader, bool pre_generated) override;
    void draw_mode(GLenum mode);
};

#endif
