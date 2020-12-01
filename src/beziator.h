#ifndef BEZIATOR_H
#define BEZIATOR_H

#include "osdo.h"

#include "shader.h"
#include "mesh.h"
#include "model.h"

typedef vec4 *surface_t[4][4];

typedef struct Beziator {
    size_t points_size, surfaces_size;
    vec4 *points;
    surface_t *surfaces;
    Mesh mesh, frame;
    Shader *editmode;
    Model model;
} Beziator;

bool beziator_init(
        Beziator *beziator, const char *name, Shader *editmode);
Beziator *beziator_create(const char *name, Shader *editmode);

void beziator_del(Beziator *beziator);
void beziator_free(Beziator *beziator);

void beziator_draw(Beziator *beziator);

bool beziator_generate(Beziator *beziator);

void beziator_save(Beziator *beziator);

static const ModelType beziator_type = {
    beziator_draw, beziator_generate, beziator_free,
};

#endif // BEZIATOR_H
