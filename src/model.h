#ifndef model_H
#define model_H

#include "osdo.h"

#include "shader.h"
#include "mesh.h"

typedef size_t surface_t[4][4];

typedef struct Model {
    char name[64];
    size_t points_size, surfaces_size;
    vec4 *points;
    surface_t *surfaces;
    Mesh mesh, frame;
    Shader *editmode;
    vec3 min_coord, max_coord;
    UT_hash_handle hh;
} Model;

bool model_init(Model *model, const char *name, Shader *editmode);

void model_del(Model *model);

void model_draw(Model *model);

bool model_generate(Model *model);

void model_save(Model *model);

static const UT_icd model_icd = {
    sizeof(Model), NULL, NULL, NULL
};

#endif // model_H
