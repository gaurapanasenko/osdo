#ifndef BEZIATOR_H
#define BEZIATOR_H

#include "osdo.h"

#include "shader.h"
#include "mesh.h"
#include "model.h"
#include "easyvector.h"

#include "EASTL/vector.h"
#include "EASTL/string.h"
using eastl::string;

typedef GLuint surfacei_t[4][4];

class Beziator : public Mesh {
public:
    typedef surfacei_t* surfaces_vector;
protected:
    const string path;
    Mesh mesh, frame, normals;
public:
    Beziator(const string& path);
    ~Beziator() override;

    bool init();

    void draw(Shader &shader) override;

    void generate() override;

    bool save();

    void rotate(size_t i);

    vector<Vertex> &get_vertices() override;
};

#endif // BEZIATOR_H
