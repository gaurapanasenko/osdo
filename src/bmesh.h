#ifndef BMESH_H
#define BMESH_H

#include <cglm/cglm.h>
#include "mesh.h"
#include "uthash.h"

typedef size_t Surface[4][4];

typedef struct BMesh {
    char name[64];
    size_t points_size, surfaces_size;
    vec4 *points;
    Surface *surfaces;
    UT_hash_handle hh;
} BMesh;

bool bmesh_init(BMesh *bmesh, const char *name);

void bmesh_del(BMesh *bmesh);

bool bmesh_generate(BMesh *bmesh, Mesh *mesh, Mesh *mesh_skel);

void bmesh_save(BMesh *bmesh);

#endif // BMESH_H
