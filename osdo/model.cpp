#include "model.h"

Model::~Model() {}

void Model::draw(Shader &, bool pre_generated) {}

void Model::generate(size_t d) {}

vector<Vertex> &Model::get_vertices() {
    return vertices;
}

void Model::edit_panel() {}
