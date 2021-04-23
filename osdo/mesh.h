#ifndef MESH_H
#define MESH_H

#include "osdo.h"
#include "model.h"
#include "easyvector.h"

#include "EASTL/vector.h"
#include "vertex.h"

class Mesh : public Model {
protected:
    vector<Vertex> vertices;
    vector<GLuint> indices;
    GLuint vao, vbo, ebo;
public:
    Mesh();
    ~Mesh() override;

    Mesh(const Mesh&) = delete;
    Mesh(Mesh&&) = delete;
    Mesh& operator=(const Mesh&) = delete;
    Mesh& operator=(Mesh&&) = delete;

    vector<Vertex>& get_vertices() override;
    void cube_update();
    void update(vector<Vertex> vertices, vector<GLuint> indices);
    void clear();

    void draw(Shader &shader) override;
    void draw_mode(GLenum mode);
};

#endif
