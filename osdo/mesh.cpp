#include <cstring>
#include "mesh.h"
#include "conf.h"

Mesh::Mesh() : indices_size(0) {
    // create buffers/arrays
    glGenVertexArrays(1, &this->vao);
    glGenBuffers(1, &this->vbo);
    glGenBuffers(1, &this->ebo);
    glBindVertexArray(this->vao);
    glBindBuffer(GL_ARRAY_BUFFER, this->vbo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->ebo);

    // set the vertex attribute pointers
    // vertex Positions
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                          reinterpret_cast<void*>(offsetof(Vertex, position)));

    // vertex normals
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                          reinterpret_cast<void*>(offsetof(Vertex, normal)));

    // vertex color
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(Vertex),
                          reinterpret_cast<void*>(offsetof(Vertex, color)));

    // vertex uv
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                          reinterpret_cast<void*>(offsetof(Vertex, uv)));

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

Mesh::~Mesh() {
    glDeleteVertexArrays(1, &this->vao);
    glDeleteBuffers(1, &this->vbo);
    glDeleteBuffers(1, &this->ebo);
}

void Mesh::cube_update() {
    size_t vert_size = sizeof(EXAMPLE_CUBE_VERTEX) / sizeof(Vertex);
    size_t indi_size = sizeof(EXAMPLE_CUBE_INDICIES) / sizeof(GLuint);
    update(EXAMPLE_CUBE_VERTEX, vert_size,
           EXAMPLE_CUBE_INDICIES, indi_size);
}

void Mesh::update(const Vertex *vertices, size_t vertices_n,
                  const GLuint *indices, size_t indices_n) {
    if (vertices_n == 0 || indices_n == 0)
        return;
    indices_size = static_cast<GLint>(indices_n);
    glBindVertexArray(this->vao);
    // load data into vertex buffers
    glBindBuffer(GL_ARRAY_BUFFER, this->vbo);
    glBufferData(GL_ARRAY_BUFFER, vertices_n * sizeof(Vertex),
                 vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices_n * sizeof(GLuint),
                 indices, GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void Mesh::draw_mode(GLenum mode) {
    if (!indices_size)
        return;
    glBindVertexArray(this->vao);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->ebo);
    glBindBuffer(GL_ARRAY_BUFFER, this->vbo);
    glDrawElements(mode, static_cast<GLint>(this->indices_size),
                   GL_UNSIGNED_INT, nullptr);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void Mesh::draw(Shader &, bool) {
    Mesh::draw_mode(GL_TRIANGLES);
}
