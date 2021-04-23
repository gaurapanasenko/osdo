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
    virtual void draw(Shader &shader);
    virtual void generate();
    virtual vector<Vertex>& get_vertices();
    virtual void edit_panel();
};

#endif // MODEL_H
