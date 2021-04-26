#ifndef BEZIATOR_H
#define BEZIATOR_H

#include <EASTL/string.h>
#include "osdo.h"
#include "mesh.h"

using eastl::string;

typedef GLuint surfacei_t[4][4];

class Beziator : public Mesh {
public:
    typedef surfacei_t* surfaces_vector;
protected:
    const string path;
    Mesh mesh, frame, normals;
    vector<Vertex> vertices;
    vector<GLuint> indices;
public:
    Beziator(const string& path);
    ~Beziator() override;

    bool init();

    void draw(Shader &shader, bool pre_generated) override;

    void generate(size_t d = 8) override;

    bool save();

    void rotate(size_t i);

    vector<Vertex> *get_vertices() override;
};

#endif // BEZIATOR_H
