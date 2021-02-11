#ifndef BEZIATOR_H
#define BEZIATOR_H

#include "osdo.h"

#include "shader.h"
#include "mesh.h"
#include "model.h"

typedef vec4 *surface_t[4][4];
typedef int surfacei_t[4][4];

typedef struct Beziator {
    char *path;
    size_t points_size, surfaces_size;
    vec4 *points;
    surfacei_t *surfaces;
    Mesh mesh, frame, normals;
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

bool beziator_save(Beziator *beziator);

static const ModelType beziator_type = {
    beziator_draw, beziator_generate, beziator_free,
};

static inline void beziator_rotate(Beziator *beziator, size_t i) {
    surfacei_t s;
    memcpy(s, beziator->surfaces[i], sizeof(surfacei_t));
    for (int k = 0; k < 4; k++)
        for (int j = 0; j < 4; j++) {
            beziator->surfaces[i][k][j] = s[j][k];
        }
}

#endif // BEZIATOR_H
