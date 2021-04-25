#ifndef MODEL_H
#define MODEL_H

#include "EASTL/vector.h"
#include "osdo.h"

using eastl::vector;

struct Vertex;
class Shader;

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
