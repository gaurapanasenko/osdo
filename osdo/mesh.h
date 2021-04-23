#ifndef MESH_H
#define MESH_H

#include "osdo.h"
#include "model.h"
#include "easyvector.h"

#include "EASTL/vector.h"
#include "vertex.h"

template<class T>
using EasyVector = eastl::vector<T>;

class Mesh : public Model {
    EasyVector<Vertex> vertices;
    EasyVector<GLuint> indices;
    GLuint vao, vbo, ebo;
public:
    Mesh();
    ~Mesh() override;
    vector<Vertex>& get_vertices() override;
    void cube_update();
    void update(EasyVector<Vertex> vertices, EasyVector<GLuint> indices);
    void clear();

    void draw(Shader &shader) override;
    void draw_mode(GLenum mode);
};

#endif
