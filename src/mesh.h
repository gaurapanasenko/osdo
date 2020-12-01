#ifndef MESH_H
#define MESH_H

#include "osdo.h"
#include "model.h"

typedef struct Vertex {
    vec3 position;
    vec3 normal;
    unsigned char color[4];
    vec2 uv;
} Vertex;

typedef struct Mesh {
    GLsizei vertices_size, indices_size;
    Vertex *vertices;
    GLuint *indices;
    GLuint vao, vbo, ebo;
} Mesh;

void mesh_init(Mesh* mesh);
void mesh_cube_update(Mesh* mesh);
Mesh *mesh_create(void);

void mesh_del(Mesh* mesh);
void mesh_free(Mesh* mesh);

void mesh_update(
        Mesh* mesh, GLsizei vertices_size, GLsizei indices_size,
        Vertex *vertices, GLuint *indices);
void mesh_clear(Mesh* mesh);

void mesh_draw(Mesh *mesh);
void mesh_draw_mode(Mesh *mesh, GLenum mode);

static const ModelType mesh_type = {
    mesh_draw, NULL, mesh_free,
};

#endif
