#include <string.h>
#include "mesh.h"
#include "conf.h"

Mesh::Mesh() {
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
    Mesh::clear();
}

vector<Vertex> &Mesh::get_vertices() {
    return vertices;
}

void Mesh::update(EasyVector<Vertex> vertices, EasyVector<GLuint> indices) {
    Mesh::clear();
    this->vertices = vertices;
    this->indices = indices;
    if (this->vertices.size() && this->indices.size()) {
        glBindVertexArray(this->vao);
        // load data into vertex buffers
        glBindBuffer(GL_ARRAY_BUFFER, this->vbo);
        glBufferData(GL_ARRAY_BUFFER,
                     this->vertices.size() * sizeof(Vertex),
                     this->vertices.data(), GL_DYNAMIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                     this->indices.size() * sizeof(GLuint),
                     this->indices.data(), GL_DYNAMIC_DRAW);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }

}

void Mesh::clear() {
    this->vertices.clear();
    this->indices.clear();
}


void Mesh::cube_update() {
    EasyVector<Vertex> V(sizeof(EXAMPLE_CUBE_VERTEX) / sizeof(Vertex));
    EasyVector<GLuint> E(sizeof(EXAMPLE_CUBE_INDICIES) / sizeof(GLuint));
    memcpy(V.data(), EXAMPLE_CUBE_VERTEX, sizeof(EXAMPLE_CUBE_VERTEX));
    memcpy(E.data(), EXAMPLE_CUBE_INDICIES, sizeof(EXAMPLE_CUBE_INDICIES));
    update(V, E);
}

void Mesh::draw_mode(GLenum mode) {
    glBindVertexArray(this->vao);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->ebo);
    glBindBuffer(GL_ARRAY_BUFFER, this->vbo);
    glDrawElements(mode, static_cast<GLint>(this->indices.size()),
                   GL_UNSIGNED_INT, nullptr);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void Mesh::draw(Shader &) {
    Mesh::draw_mode(GL_TRIANGLES);
}
