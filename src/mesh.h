#ifndef MESH_H
#define MESH_H

#include <cglm/cglm.h>
#include <GL/gl.h>

typedef struct Vertex {
    vec3 Position;
    vec3 Normal;
    vec3 Color;
} Vertex;

typedef struct Mesh {
    GLsizei vertices_size, indices_size;
    Vertex *vertices;
    GLuint *indices, VAO, VBO, EBO;
} Mesh;

void mesh_subinit(Mesh* mesh, GLsizei vertices_size, GLsizei indices_size,
                  Vertex *vertices, GLuint *indices);

void mesh_init(Mesh* mesh);

void mesh_draw(Mesh* mesh);

void mesh_del(Mesh* mesh);

#endif
