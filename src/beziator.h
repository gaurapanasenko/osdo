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

typedef vec4 *surface_t[4][4];
typedef int surfacei_t[4][4];

class Beziator : public Model {
public:
    typedef EasyVector<vec4> points_vector;
    typedef EasyVector<surfacei_t> surfaces_vector;
private:
    char name[64];
    char *path;
    points_vector points;
    surfaces_vector surfaces;
    Mesh mesh, frame, normals;
    shared_ptr<Shader> editmode;
    Model model;
public:
    Beziator(const char *name, shared_ptr<Shader> editmode);
    ~Beziator() override;

    bool init();

    void draw() override;

    void generate() override;

    bool save();

    void rotate(size_t i);

    void edit_panel(mat4 matr);

    points_vector &get_points();
    surfaces_vector &get_surfaces();
};

#endif // BEZIATOR_H
