#include "model.h"

Model::~Model() {}

void Model::draw(Shader &) {}

void Model::generate() {}

vector<Vertex> &Model::get_vertices() {
    return vertices;
}

void Model::edit_panel() {}
