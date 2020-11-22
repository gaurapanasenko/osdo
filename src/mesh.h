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
    GLuint *indices;
} Mesh;

typedef struct GlMesh {
    Mesh *mesh;
    GLuint VAO, VBO, EBO;
} GlMesh;

#define MESH_INIT {0, 0, NULL, NULL}
#define GLMESH_INIT {NULL, 0, 0, 0}

void mesh_subinit(Mesh* mesh, GLsizei vertices_size, GLsizei indices_size,
                  Vertex *vertices, GLuint *indices);

void mesh_init(Mesh* mesh);

void mesh_draw(GlMesh *mesh);

void mesh_del(Mesh* mesh);

GlMesh mesh_bind(Mesh* mesh);

void mesh_unbind(GlMesh* mesh);

#endif
