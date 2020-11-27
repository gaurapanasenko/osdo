#ifndef BMESH_H
#define BMESH_H

#include <cglm/cglm.h>
#include "mesh.h"
#include "uthash.h"
#include "utarray.h"

typedef size_t Surface[4][4];

typedef struct BMesh {
    char name[64];
    size_t points_size, surfaces_size;
    vec4 *points;
    Surface *surfaces;
    Mesh *mesh, *mesh_skel;
    UT_hash_handle hh;
} BMesh;

bool bmesh_init(BMesh *bmesh, const char *name, Mesh *mesh, Mesh *mesh_skel);

void bmesh_del(BMesh *bmesh);

bool bmesh_generate(BMesh *bmesh);

void bmesh_save(BMesh *bmesh);

static const UT_icd bmesh_icd = {
    sizeof(BMesh), NULL, NULL, NULL
};

#endif // BMESH_H
