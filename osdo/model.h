#ifndef MODEL_H
#define MODEL_H

#include <EASTL/vector.h>
#include "osdo.h"

struct Vertex;
class Shader;
using eastl::vector;

class Model {
public:
    virtual ~Model();
    virtual void draw(Shader &shader, bool pre_generated = false);
    virtual void generate(size_t d = 8);
    virtual vector<Vertex> *get_vertices();
    virtual void edit_panel();
};

#endif // MODEL_H
