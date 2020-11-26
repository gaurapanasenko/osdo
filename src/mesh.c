#include <string.h>
#include <GL/glew.h>
#include "mesh.h"
#include "conf.h"

void mesh_subinit(Mesh *mesh, GLsizei vertices_size, GLsizei indices_size, Vertex *vertices, GLuint *indices) {
    mesh->vertices_size = vertices_size;
    mesh->indices_size = indices_size;
    mesh->vertices = vertices;
    mesh->indices = indices;

    // create buffers/arrays
    glGenVertexArrays(1, &mesh->VAO);
    glGenBuffers(1, &mesh->VBO);
    glGenBuffers(1, &mesh->EBO);

    glBindVertexArray(mesh->VAO);
    // load data into vertex buffers
    glBindBuffer(GL_ARRAY_BUFFER, mesh->VBO);
    glBufferData(GL_ARRAY_BUFFER,
                 (size_t)mesh->vertices_size * sizeof(Vertex),
                 mesh->vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                 (size_t)mesh->indices_size * sizeof(GLuint),
                 mesh->indices, GL_STATIC_DRAW);

    // set the vertex attribute pointers
    // vertex Positions
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                          (void*)offsetof(Vertex, Position));

    // vertex normals
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                          (void*)offsetof(Vertex, Normal));

    // vertex color
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                          (void*)offsetof(Vertex, Color));

    glBindVertexArray(0);
}

void mesh_init(Mesh* mesh) {
    Vertex *V = (Vertex*)malloc(sizeof(EXAMPLE_CUBE_VERTEX));
    GLuint *E = (GLuint*)malloc(sizeof(EXAMPLE_CUBE_INDICIES));
    memcpy(V, EXAMPLE_CUBE_VERTEX, sizeof(EXAMPLE_CUBE_VERTEX));
    memcpy(E, EXAMPLE_CUBE_INDICIES, sizeof(EXAMPLE_CUBE_INDICIES));
    mesh_subinit(mesh, sizeof(EXAMPLE_CUBE_VERTEX)/sizeof(Vertex),
                 sizeof(EXAMPLE_CUBE_INDICIES)/sizeof(GLuint), V, E);
}

void mesh_draw(Mesh *mesh) {
    glBindVertexArray(mesh->VAO);
    glDrawElements(GL_TRIANGLES, mesh->indices_size,
                   GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

void mesh_del(Mesh *mesh) {
    glDeleteVertexArrays(1, &mesh->VAO);
    glDeleteBuffers(1, &mesh->VBO);
    glDeleteBuffers(1, &mesh->EBO);
    free(mesh->vertices);
    free(mesh->indices);
}
