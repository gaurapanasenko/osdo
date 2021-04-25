#ifndef MODEL_H
#define MODEL_H

#include "osdo.h"
#include "vertex.h"
#include "shader.h"
#include "EASTL/vector.h"
using eastl::vector;

class Model {
    vector<Vertex> vertices;
public:
    virtual ~Model();
    virtual void draw(Shader &shader, bool pre_generated = false);
    virtual void generate(size_t d = 8);
    virtual vector<Vertex>& get_vertices();
    virtual void edit_panel();
};

#endif // MODEL_H
