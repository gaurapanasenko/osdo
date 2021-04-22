#ifndef MESH_H
#define MESH_H

#include "osdo.h"
#include "model.h"
#include "easyvector.h"

#include "EASTL/vector.h"

template<class T>
using EasyVector = eastl::vector<T>;

enum VetrexType {
    VERTEX_SIMPLE = 0,
    VERTEX_BEZIER = 1,
};

struct Vertex {
    VetrexType type;
    int node_id;
    vec3 position;
    vec3 normal;
    unsigned char color[4];
    vec2 uv;
    /*Vertex(VetrexType type, int node_id, vec3 position,
           vec3 normal, unsigned char color[4], vec2 uv)
        : type(type), node_id(node_id)
    {}*/
};

class Mesh : public Model {
    EasyVector<Vertex> vertices;
    EasyVector<GLuint> indices;
    GLuint vao, vbo, ebo;
public:
    Mesh();
    ~Mesh() override;
    void cube_update();
    void update(EasyVector<Vertex> vertices, EasyVector<GLuint> indices);
    void clear();

    void draw() override;
    void draw_mode(GLenum mode);
};

#endif
