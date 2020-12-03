#include "beziator.h"
#include "conf.h"

#define BEZIATOR_PATH RES_DIR"/%s.odom"
#define BEZIER_TANGENT_INIT {\
{  0,  0,  0,  0},\
{ -3,  9, -9,  3},\
{  6,-12,  6,  0},\
{ -3,  3,  0,  0},}
#define DETALIZATION 16

bool beziator_init(
        Beziator *beziator, const char *name, Shader *editmode) {
    mesh_init(&beziator->mesh);
    mesh_init(&beziator->frame);
    beziator->editmode = editmode;
    const size_t path_len = strlen(BEZIATOR_PATH);
    const size_t len = strlen(BEZIATOR_PATH);
    char *path = calloc(len + path_len, sizeof(char));
    snprintf(path, len + path_len, BEZIATOR_PATH, name);
    FILE *file = fopen(path, "r");
    if (file == NULL) {
        printf("ERROR: failed to open file %s\n", path);
        free(path);
        return false;
    }
    free(path);

    fscanf(file, "%lu%lu", &beziator->points_size, &beziator->surfaces_size);
    beziator->points = (vec4*)calloc(beziator->points_size, sizeof(vec4));
    if (beziator->points == NULL) {
        fclose(file);
        printf("Failed to allocate memory.\n");
        return false;
    }
    beziator->surfaces = (surface_t*)calloc(beziator->surfaces_size, sizeof(surface_t));
    if (beziator->points == NULL) {
        fclose(file);
        free(beziator->points);
        printf("Failed to allocate memory.\n");
        return false;
    }
    vec4 *point;
    surface_t *surface;
    for (size_t i = 0; i < beziator->points_size; i++) {
        point = &beziator->points[i];
        glm_vec4_copy(GLM_VEC4_BLACK, *point);
        fscanf(file, "%f%f%f", (*point), (*point) + 1, (*point) + 2);
    }
    int j, k;
    size_t tmp;
    for (size_t i = 0; i < beziator->surfaces_size; i++) {
        surface = &beziator->surfaces[i];
        for (j = 0; j < 4; j++)
            for (k = 0; k < 4; k++) {
                fscanf(file, "%lu", &tmp);
                (*surface)[j][k] = beziator->points + tmp;
            }
    }
    fclose(file);
    return true;
}

Beziator *beziator_create(const char *name, Shader *editmode) {
    Beziator *beziator = calloc(1, sizeof(Beziator));
    beziator_init(beziator, name, editmode);
    return beziator;
}

void beziator_del(Beziator *beziator) {
    if (beziator->points) {
        free(beziator->points);
        beziator->points = NULL;
    }
    if (beziator->surfaces) {
        free(beziator->surfaces);
        beziator->surfaces = NULL;
    }
    mesh_del(&beziator->mesh);
    mesh_del(&beziator->frame);
}

void beziator_free(Beziator *beziator) {
    beziator_del(beziator);
    free(beziator);
}

void beziator_draw(Beziator *beziator) {
    //shader_set_vec3(beziator->editmode, "min_coord", beziator->min_coord);
    //shader_set_vec3(beziator->editmode, "max_coord", beziator->max_coord);
    mesh_draw_mode(&beziator->frame, GL_POINTS);
    mesh_draw_mode(&beziator->frame, GL_LINES);
    shader_set_float(beziator->editmode, "alpha", 0.5f);
    mesh_draw_mode(&beziator->mesh, GL_TRIANGLES);
    shader_set_float(beziator->editmode, "alpha", 1);
}


void bezier_curve(float a, mat4 points, vec4 dest) {
    mat4 matrix;
    glm_vec4_cubic(a, dest);
    glm_mat4_mul(points, GLM_BEZIER_MAT, matrix);
    glm_mat4_mulv(matrix, dest, dest);
}

void bezier_curve_tangent(float a, mat4 points, vec4 dest) {
    mat4 matrix;
    glm_vec4_cubic(a, dest);
    glm_mat4_mul(points, (mat4)BEZIER_TANGENT_INIT, matrix);
    glm_mat4_mulv(matrix, dest, dest);
}

void bezier_surface(
        float u, float v, surface_t points, vec4 dest, vec4 normal) {
    mat4 m, res1, res2, res3;

    for (int i = 0; i < 4; i++) {
        glm_vec4_copy(*(points[0][i]), m[0]);
        glm_vec4_copy(*(points[1][i]), m[1]);
        glm_vec4_copy(*(points[2][i]), m[2]);
        glm_vec4_copy(*(points[3][i]), m[3]);
        bezier_curve(u, m, res1[i]);

        glm_vec4_copy(*(points[i][0]), m[0]);
        glm_vec4_copy(*(points[i][1]), m[1]);
        glm_vec4_copy(*(points[i][2]), m[2]);
        glm_vec4_copy(*(points[i][3]), m[3]);
        bezier_curve(v, m, res2[i]);
    }

    bezier_curve(v, res1, dest);
    bezier_curve_tangent(v, res1, res3[1]);
    bezier_curve_tangent(u, res2, res3[3]);

    glm_cross(res3[3], res3[1], normal);
}

bool beziator_generate(Beziator *beziator) {
    static const int controls_lines[][2] = {
        {0, 0}, {0, 1}, {0, 0}, {1, 1}, {0, 0}, {1, 0},
        {0, 3}, {0, 2}, {0, 3}, {1, 2}, {0, 3}, {1, 3},
        {3, 0}, {2, 0}, {3, 0}, {2, 1}, {3, 0}, {3, 1},
        {3, 3}, {3, 2}, {3, 3}, {2, 2}, {3, 3}, {2, 3},
    };
    static const int ctrls_size = sizeof(controls_lines) / sizeof(int[2]);
    /* // Old variant of config, I leave it for several commits
    static const uint8_t ALL_SQUARE_LINES[][4] = {
        {1, 0, 0, 0}, {0, 0, 0, 1}, {0, 1, 1, 1}, {1, 1, 1, 0},
        {1, 1, 0, 0}, {0, 0, 1, 1}, {1, 0, 0, 1}, {0, 1, 1, 0}
    };*/
    static const uint8_t SQUARE_TYPES[][10][2] = {
        {{0, 0}, {0, 1}, {1, 1}, {0, 0}, {8, 8},
         {1, 1}, {1, 0}, {0, 0}, {1, 1}, {9, 9}},
        {{0, 0}, {0, 1}, {1, 1}, {1, 0}, {0, 0}, {9, 9}},
        {{1, 0}, {0, 0}, {0, 1}, {1, 0}, {8, 8},
         {0, 1}, {1, 1}, {1, 0}, {0, 1}, {9, 9}},
        /* // And again old variant of config
        {1, 2, 4, 1, 8, 0, 5, 3, 0, 9},
        {0, 1, 2, 3, 0, 9},
        {0, 1, 5, 0, 8, 2, 3, 5, 2, 9},*/
    };
    static const uint8_t BEZIER_SQUARE_TYPES[3][3] = {
        {0, 1, 2}, {1, 1, 1}, {2, 1, 0}
    };

    size_t j, k, index;
    float x, u, v;
    vec4 *point, vertex, normal;
    surface_t *surface;
    GLuint verts = 0, verts2 = 0;
    const int *c;
    mat4 m4b;
    uint8_t si, sj;
    const uint8_t (*st)[2];

    Mesh *mesh = &beziator->mesh, *mesh_skel = &beziator->frame;
    const size_t d = DETALIZATION;
    x = 1.f / (d - 1);

    const unsigned long size =
            6 * 9 * d * d * beziator->surfaces_size;
    const GLsizei sizei = (GLsizei)size;
    Vertex *V = (Vertex*)calloc(size, sizeof(Vertex));
    GLuint *E = (GLuint*)calloc(size, sizeof(GLuint));
    Vertex *V2 = (Vertex*)calloc(size, sizeof(Vertex));
    GLuint *E2 = (GLuint*)calloc(size, sizeof(GLuint));

    // Creator frame vertices
    for (size_t i = 0; i < beziator->points_size; i++) {
        point = &beziator->points[i];
        glm_vec3_copy(*point, V2[i].position);
        V2[i].color[1] = 255;
        V2[i].color[3] = 255;
    }

    for (size_t i = 0; i < beziator->surfaces_size; i++) {
        // Creator frame lines
        for (j = 0; j < ctrls_size; j++) {
            c = controls_lines[j];
            E2[verts2++] =
                    (unsigned)(beziator->surfaces[i][c[0]][c[1]] - beziator->points);
        }

        // Create vertices
        for (j = 0; j < d; j++) {
            for (k = 0; k < d; k++) {
                u = (float)j*x; v = (float)k*x;
                index = i * d * d + j * d + k;
                bezier_surface(u, v, beziator->surfaces[i], vertex, normal);
                glm_vec3_copy(vertex, V[index].position);
                glm_vec3_copy(normal, V[index].normal);
                V[verts].color[1] = 255;
            }
        }

        // Create triangles
        for (j = 0; j < d - 1; j++)
            for (k = 0; k < d - 1; k++) {
                E[verts++] = (unsigned)(i * d * d + j * d + k);
                E[verts++] = (unsigned)(i * d * d + (j + 1) * d + k);
                E[verts++] = (unsigned)(i * d * d + j * d + k + 1);

                E[verts++] = (unsigned)(i * d * d + (j + 1) * d + k + 1);
                E[verts++] = (unsigned)(i * d * d + j * d + k + 1);
                E[verts++] = (unsigned)(i * d * d + (j + 1) * d + k);
            }

        surface = &(beziator->surfaces[i]);
        for (si = 0; si < 3; si++) {
            for (sj = 0; sj < 3; sj++) {
                st = SQUARE_TYPES[BEZIER_SQUARE_TYPES[si][sj]];
                while (st[2][0] != 9) {
                    if (st[2][0] == 8) {
                        st += 3;
                    }
                    index = (size_t)((*surface)[si+st[1][0]][sj+st[1][1]] - beziator->points);
                    glm_vec3_sub(*((*surface)[si+st[1][0]][sj+st[1][1]]),
                            *((*surface)[si+st[0][0]][sj+st[0][1]]), m4b[0]);
                    glm_vec3_sub(*((*surface)[si+st[1][0]][sj+st[1][1]]),
                            *((*surface)[si+st[2][0]][sj+st[2][1]]), m4b[1]);
                    glm_vec3_cross(m4b[0], m4b[1], m4b[2]);
                    glm_vec3_add(V2[index].normal, m4b[2], V2[index].normal);
                    st++;
                }
            }
        }
    }
    /*
    // Example drawing of normals for frame
    for (size_t i = 0; i < beziator->points_size; i++) {
        glm_normalize(V2[i].normal);
        glm_vec3_add(V2[i].position, V2[i].normal,
                     V2[i+beziator->points_size].position);
        E2[verts2++] = (unsigned)i;
        E2[verts2++] = (unsigned)(i+beziator->points_size);
    }*/
    mesh_update(mesh, sizei, sizei, V, E);
    mesh_update(mesh_skel, sizei, sizei, V2, E2);
    return true;
}
