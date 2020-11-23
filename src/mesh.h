#ifndef MESH_H
#define MESH_H

#include <cglm/cglm.h>
#include <GL/gl.h>
#include <uthash.h>

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
    UT_hash_handle hh;
} GlMesh;

#define MESH_INIT {0, 0, NULL, NULL}

void mesh_subinit(Mesh* mesh, GLsizei vertices_size, GLsizei indices_size,
                  Vertex *vertices, GLuint *indices);

void mesh_init(Mesh* mesh);

void mesh_draw(GlMesh *mesh);

void mesh_del(Mesh* mesh);

void mesh_bind(Mesh* mesh, GlMesh* gm);

void mesh_unbind(GlMesh* mesh);

#endif
