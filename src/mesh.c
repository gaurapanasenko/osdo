#include <string.h>
#include "mesh.h"
#include "conf.h"

void mesh_subinit(Mesh *mesh, const char *name) {
    strcpy(mesh->name, name);
    mesh->vertices_size = 0;
    mesh->indices_size = 0;
    mesh->vertices = NULL;
    mesh->indices = NULL;

    // create buffers/arrays
    glGenVertexArrays(1, &mesh->vao);
    glGenBuffers(1, &mesh->vbo);
    glGenBuffers(1, &mesh->ebo);
    glBindVertexArray(mesh->vao);
    glBindBuffer(GL_ARRAY_BUFFER, mesh->vbo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->ebo);

    // set the vertex attribute pointers
    // vertex Positions
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                          (void*)offsetof(Vertex, position));

    // vertex normals
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                          (void*)offsetof(Vertex, normal));

    // vertex color
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(Vertex),
                          (void*)offsetof(Vertex, color));

    // vertex uv
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                          (void*)offsetof(Vertex, uv));

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void mesh_update(Mesh* mesh, GLsizei vertices_size, GLsizei indices_size, Vertex *vertices, GLuint *indices) {
    if (vertices != NULL && indices != NULL) {
        glBindVertexArray(mesh->vao);
        // load data into vertex buffers
        glBindBuffer(GL_ARRAY_BUFFER, mesh->vbo);
        glBufferData(GL_ARRAY_BUFFER,
                     (size_t)vertices_size * sizeof(Vertex),
                     vertices, GL_DYNAMIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                     (size_t)indices_size * sizeof(GLuint),
                     indices, GL_DYNAMIC_DRAW);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }

    mesh_clear(mesh);
    mesh->vertices_size = vertices_size;
    mesh->indices_size = indices_size;
    mesh->vertices = vertices;
    mesh->indices = indices;
}

void mesh_clear(Mesh* mesh) {
    if (mesh->vertices) {
        free(mesh->vertices);
        mesh->vertices = NULL;
    }
    if (mesh->indices) {
        free(mesh->indices);
        mesh->indices = NULL;
    }
    mesh->vertices_size = 0;
    mesh->indices_size = 0;
}


void mesh_init(Mesh* mesh, const char *name) {
    //strcpy(mesh->name, name);
    //Vertex *V = (Vertex*)malloc(sizeof(EXAMPLE_CUBE_VERTEX));
    //GLuint *E = (GLuint*)malloc(sizeof(EXAMPLE_CUBE_INDICIES));
    //memcpy(V, EXAMPLE_CUBE_VERTEX, sizeof(EXAMPLE_CUBE_VERTEX));
    //memcpy(E, EXAMPLE_CUBE_INDICIES, sizeof(EXAMPLE_CUBE_INDICIES));
    //mesh_subinit(mesh, sizeof(EXAMPLE_CUBE_VERTEX)/sizeof(Vertex),
    //             sizeof(EXAMPLE_CUBE_INDICIES)/sizeof(GLuint), V, E);
    mesh_subinit(mesh, name);
}

void mesh_draw(Mesh *mesh) {
    glBindVertexArray(mesh->vao);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->ebo);
    glBindBuffer(GL_ARRAY_BUFFER, mesh->vbo);
    glDrawElements(GL_TRIANGLES, mesh->indices_size,
                   GL_UNSIGNED_INT, 0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void mesh_del(Mesh *mesh) {
    glDeleteVertexArrays(1, &mesh->vao);
    glDeleteBuffers(1, &mesh->vbo);
    glDeleteBuffers(1, &mesh->ebo);
    mesh_clear(mesh);
}
