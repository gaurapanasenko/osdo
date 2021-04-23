#ifndef BEZIATOR_H
#define BEZIATOR_H

#include "osdo.h"

#include "shader.h"
#include "mesh.h"
#include "model.h"
#include "easyvector.h"

#include "EASTL/vector.h"

template<class T>
using EasyVector = eastl::vector<T>;

typedef size_t surfacei_t[4][4];

class Beziator : public Model {
public:
    typedef EasyVector<surfacei_t> surfaces_vector;
protected:
    char name[64];
    char *path;
    vector<Vertex> points;
    surfaces_vector surfaces;
    Mesh mesh, frame, normals;
    Model model;
public:
    Beziator(const char *name);
    ~Beziator() override;

    bool init();

    void draw(Shader &shader) override;

    void generate() override;

    bool save();

    void rotate(size_t i);

    vector<Vertex> &get_vertices() override;
    surfaces_vector &get_surfaces();
};

#endif // BEZIATOR_H
