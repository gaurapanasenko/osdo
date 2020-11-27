#include "bmesh.h"
#include "conf.h"

#define BMESH_PATH RES_DIR"/%s.odom"

bool bmesh_init(BMesh *bmesh, const char *name, Mesh *mesh, Mesh *mesh_skel) {
    bmesh->mesh = mesh;
    bmesh->mesh_skel = mesh_skel;
    const size_t path_len = strlen(BMESH_PATH);
    const size_t len = strlen(name);
    strcpy(bmesh->name, name);
    char *path = calloc(len + path_len, sizeof(char));
    snprintf(path, len + path_len, BMESH_PATH, name);
    FILE *file = fopen(path, "r");
    if (file == NULL) {
        printf("ERROR: failed to open file %s\n", path);
        free(path);
        return false;
    }
    free(path);

    fscanf(file, "%lu%lu", &bmesh->points_size, &bmesh->surfaces_size);
    bmesh->points = (vec4*)calloc(bmesh->points_size, sizeof(vec4));
    if (bmesh->points == NULL) {
        fclose(file);
        printf("Failed to allocate memory.\n");
        return false;
    }
    bmesh->surfaces = (Surface*)calloc(bmesh->surfaces_size, sizeof(Surface));
    if (bmesh->points == NULL) {
        fclose(file);
        free(bmesh->points);
        printf("Failed to allocate memory.\n");
        return false;
    }
    vec4 *point;
    Surface *surface;
    for (size_t i = 0; i < bmesh->points_size; i++) {
        point = &bmesh->points[i];
        glm_vec4_copy(GLM_VEC4_BLACK, *point);
        fscanf(file, "%f%f%f", (*point), (*point) + 1, (*point) + 2);
    }
    int j, k;
    for (size_t i = 0; i < bmesh->surfaces_size; i++) {
        surface = &bmesh->surfaces[i];
        for (j = 0; j < 4; j++)
            for (k = 0; k < 4; k++)
                fscanf(file, "%lu", (*surface)[j] + k);
    }
    fclose(file);
    return true;
}

void bmesh_del(BMesh *bmesh) {
    if (bmesh->points) {
        free(bmesh->points);
        bmesh->points = NULL;
    }
    if (bmesh->surfaces) {
        free(bmesh->surfaces);
        bmesh->surfaces = NULL;
    }
}

void part(float a, vec4 p1, vec4 p2, vec4 dest) {
    glm_vec4_copy(GLM_VEC4_BLACK, dest);
    glm_vec4_muladds(p1, a, dest);
    glm_vec4_muladds(p2, 1.f - a, dest);
}

void bezier(float a, vec4 p1, vec4 p2, vec4 p3, vec4 p4, vec4 dest) {
    vec4 pp1, pp2, pp3, ppp1, ppp2;
    part(a, p1, p2, pp1);
    part(a, p2, p3, pp2);
    part(a, p3, p4, pp3);
    part(a, pp1, pp2, ppp1);
    part(a, pp2, pp3, ppp2);
    part(a, ppp1, ppp2, dest);
}

bool bmesh_generate(BMesh *bmesh) {
    Mesh *mesh = bmesh->mesh, *mesh_skel = bmesh->mesh_skel;
    //const size_t size_name = strlen(bmesh->name);
    strcpy(bmesh->name, mesh->name);
    //strcpy(bmesh->name, mesh_skel->name);
    //strcpy(mesh_skel->name + size_name, "_wire");
    const unsigned long size = 6 * 9 * 16 * bmesh->surfaces_size;
    const GLsizei sizei = (GLsizei)size;
    Vertex *V = (Vertex*)calloc(size, sizeof(Vertex));
    GLuint *E = (GLuint*)calloc(size, sizeof(GLuint));
    Vertex *V2 = (Vertex*)calloc(size, sizeof(Vertex));
    GLuint *E2 = (GLuint*)calloc(size, sizeof(GLuint));
    int j, k, l;
    GLuint verts = 0, verts2 = 0;
    vec4 s[8][8], v[4]; float x = 1.f/7;
    for (size_t i = 0; i < bmesh->surfaces_size; i++) {
        for (j = 0; j < 8; j++) {
            for (k = 0; k < 8; k++) {
                for (l = 0; l < 4; l++) {
                    bezier((float)j*x, bmesh->points[bmesh->surfaces[i][0][l]],
                            bmesh->points[bmesh->surfaces[i][1][l]],
                            bmesh->points[bmesh->surfaces[i][2][l]],
                            bmesh->points[bmesh->surfaces[i][3][l]],
                            v[l]);
                }
                bezier((float)k*x, v[0], v[1], v[2], v[3], s[j][k]);
            }
        }
        for (j = 0; j < 7; j++)
            for (k = 0; k < 7; k++) {
                glm_vec3_copy(s[j][k],V[verts].position);
                V[verts].color[1] = s[j][k][2] * 100;
                E[verts] = verts;
                verts++;
                glm_vec3_copy(s[j+1][k],V[verts].position);
                V[verts].color[1] = s[j+1][k][2] * 100;
                E[verts] = verts;
                verts++;
                glm_vec3_copy(s[j][k+1],V[verts].position);
                V[verts].color[1] = s[j][k+1][2] * 100;
                E[verts] = verts;
                verts++;
                glm_vec3_copy(s[j+1][k],V[verts].position);
                V[verts].color[1] = s[j+1][k][2] * 100;
                E[verts] = verts;
                verts++;
                glm_vec3_copy(s[j][k+1],V[verts].position);
                V[verts].color[1] = s[j][k+1][2] * 100;
                E[verts] = verts;
                verts++;
                glm_vec3_copy(s[j+1][k+1],V[verts].position);
                V[verts].color[1] = s[j+1][k+1][2] * 100;
                E[verts] = verts;
                verts++;

            }
        for (j = 0; j < 3; j++)
            for (k = 0; k < 3; k++) {

        glm_vec3_copy(bmesh->points[bmesh->surfaces[i][j][k]],V2[verts2].position);
        V2[verts2].color[1] = bmesh->points[bmesh->surfaces[i][j][k]][2] * 100;
        E2[verts2] = verts2;
        verts2++;
        glm_vec3_copy(bmesh->points[bmesh->surfaces[i][j+1][k]],V2[verts2].position);
        V2[verts2].color[1] = bmesh->points[bmesh->surfaces[i][j+1][k]][2] * 100;
        E2[verts2] = verts2;
        verts2++;
        glm_vec3_copy(bmesh->points[bmesh->surfaces[i][j][k+1]],V2[verts2].position);
        V2[verts2].color[1] = bmesh->points[bmesh->surfaces[i][j][k+1]][2] * 100;
        E2[verts2] = verts2;
        verts2++;
        glm_vec3_copy(bmesh->points[bmesh->surfaces[i][j+1][k]],V2[verts2].position);
        V2[verts2].color[1] = bmesh->points[bmesh->surfaces[i][j+1][k]][2] * 100;
        E2[verts2] = verts2;
        verts2++;
        glm_vec3_copy(bmesh->points[bmesh->surfaces[i][j][k+1]],V2[verts2].position);
        V2[verts2].color[1] = bmesh->points[bmesh->surfaces[i][j][k+1]][2] * 100;
        E2[verts2] = verts2;
        verts2++;
        glm_vec3_copy(bmesh->points[bmesh->surfaces[i][j+1][k+1]],V2[verts2].position);
        V2[verts2].color[1] = bmesh->points[bmesh->surfaces[i][j+1][k+1]][2] * 100;
        E2[verts2] = verts2;
        verts2++;
            }
    }
    mesh_update(mesh, sizei, sizei, V, E);
    mesh_update(mesh_skel, sizei, sizei, V2, E2);
    return true;
}
