#include "beziator.h"
#include "conf.h"

#define BEZIATOR_PATH RES_DIR"/%s.odom"

bool beziator_init(
        Beziator *beziator, const char *name, Shader *editmode) {
    mesh_init(&beziator->mesh);
    mesh_init(&beziator->frame);
    mesh_init(&beziator->mesh3);
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
    for (size_t i = 0; i < beziator->surfaces_size; i++) {
        surface = &beziator->surfaces[i];
        for (j = 0; j < 4; j++)
            for (k = 0; k < 4; k++)
                fscanf(file, "%lu", (*surface)[j] + k);
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
    //mesh_draw(&beziator->frame, GL_POINTS);
    //mesh_draw_mode(&beziator->frame, GL_LINES);
    mesh_draw_mode(&beziator->mesh3, GL_LINES);
    mesh_draw_mode(&beziator->mesh, GL_POINTS);
}

void part(float a, vec4 p1, vec4 p2, vec4 dest) {
    glm_vec4_copy(GLM_VEC4_BLACK, dest);
    glm_vec4_muladds(p1, a, dest);
    glm_vec4_muladds(p2, 1.f - a, dest); //p1*a + p2*(1-a)
}

void bezier(float a, vec4 p1, vec4 p2, vec4 p3, vec4 p4, vec4 dest, vec4 dest2, vec4 dest3) {
    vec4 pp1, pp2, pp3, ppp1, ppp2;
    part(a, p1, p2, pp1);
    part(a, p2, p3, pp2);
    part(a, p3, p4, pp3);
    part(a, pp1, pp2, dest2);
    part(a, pp2, pp3, dest3);
    part(a, dest2, dest3, dest);
    //glm_vec3_sub(ppp1, ppp2, tangent);
}

bool beziator_generate(Beziator *beziator) {
    Mesh *mesh = &beziator->mesh, *mesh_skel = &beziator->frame;
    Mesh *mesh3 = &beziator->mesh3;
    //const size_t size_name = strlen(beziator->name);
    //strcpy(beziator->name, mesh_skel->name);
    //strcpy(mesh_skel->name + size_name, "_wire");
    const unsigned long size = 6 * 9 * 16 * beziator->surfaces_size;
    const GLsizei sizei = (GLsizei)size;
    Vertex *V = (Vertex*)calloc(size, sizeof(Vertex));
    GLuint *E = (GLuint*)calloc(size, sizeof(GLuint));
    Vertex *V2 = (Vertex*)calloc(size, sizeof(Vertex));
    GLuint *E2 = (GLuint*)calloc(size, sizeof(GLuint));
    Vertex *V3 = (Vertex*)calloc(size, sizeof(Vertex));
    GLuint *E3 = (GLuint*)calloc(size, sizeof(GLuint));
    int j, k, l;
    GLuint verts = 0, verts3 = 0;// verts2 = 0;
    vec4 s[8][8], s2[8][8]; float x = 1.f/3;
    vec3 min_coord, max_coord, norms[8][8];
    glm_vec3_copy(beziator->points[0], min_coord);
    glm_vec3_copy(beziator->points[0], max_coord);
    vec4 *point;
    surface_t *surface;
    mat3 m3b;
    mat4 u[5], u2[5];
    for (size_t i = 0; i < beziator->points_size; i++) {
        point = &beziator->points[i];
        glm_vec3_copy(*point, V2[i].position);
        V2[i].color[1] = 255;
        V2[i].color[3] = 255;
        E2[i] = (unsigned)i;
        for (int j = 0; j < 3; j++) {
            if ((*point)[j] < min_coord[j])
                min_coord[j] = (*point)[j];
            if ((*point)[j] > max_coord[j])
                max_coord[j] = (*point)[j];
        }
    }
    static const int norm_conf[][6] = {
        {0, 0,  1, 0,  1, 1},
        {0, 0,  1, 1,  0, 1},

        {0, 1,  1, 1,  0, 2},

        {0, 2,  1, 2,  0, 3},

        {0, 3,  0, 2,  1, 2},


        {1, 0,  2, 0,  1, 1},
        {1, 0,  1, 1,  0, 0},


        {1, 1,  0, 0,  1, 0},
        {1, 1,  0, 1,  0, 0},
        {1, 1,  1, 2,  0, 1},
        {1, 1,  2, 1,  1, 2},
        {1, 1,  1, 0,  2, 1},


        {1, 2,  0, 2,  1, 1},
        {1, 2,  0, 3,  0, 2},
        {1, 2,  1, 3,  0, 3},
        {1, 2,  2, 2,  1, 3},
        {1, 2,  1, 1,  2, 2},
    };
    size_t tmp;
    glm_vec3_copy(min_coord, beziator->min_coord);
    glm_vec3_copy(max_coord, beziator->max_coord);
    for (size_t i = 0; i < beziator->surfaces_size; i++) {
        surface = &(beziator->surfaces[i]);
        E2[verts++] = (unsigned)((*surface)[0][0]);
        E2[verts++] = (unsigned)((*surface)[0][1]);
        E2[verts++] = (unsigned)((*surface)[0][0]);
        E2[verts++] = (unsigned)((*surface)[1][1]);
        E2[verts++] = (unsigned)((*surface)[0][0]);
        E2[verts++] = (unsigned)((*surface)[1][0]);

        E2[verts++] = (unsigned)((*surface)[0][3]);
        E2[verts++] = (unsigned)((*surface)[0][2]);
        E2[verts++] = (unsigned)((*surface)[0][3]);
        E2[verts++] = (unsigned)((*surface)[1][2]);
        E2[verts++] = (unsigned)((*surface)[0][3]);
        E2[verts++] = (unsigned)((*surface)[1][3]);

        E2[verts++] = (unsigned)((*surface)[3][0]);
        E2[verts++] = (unsigned)((*surface)[2][0]);
        E2[verts++] = (unsigned)((*surface)[3][0]);
        E2[verts++] = (unsigned)((*surface)[2][1]);
        E2[verts++] = (unsigned)((*surface)[3][0]);
        E2[verts++] = (unsigned)((*surface)[3][1]);

        E2[verts++] = (unsigned)((*surface)[3][3]);
        E2[verts++] = (unsigned)((*surface)[3][2]);
        E2[verts++] = (unsigned)((*surface)[3][3]);
        E2[verts++] = (unsigned)((*surface)[2][2]);
        E2[verts++] = (unsigned)((*surface)[3][3]);
        E2[verts++] = (unsigned)((*surface)[2][3]);

        for (j = 0; j < 12; j++) {
            tmp = beziator->surfaces[i][norm_conf[j][0]][norm_conf[j][1]];
            glm_vec3_sub(beziator->points[tmp], beziator->points[beziator->surfaces[i][norm_conf[j][2]][norm_conf[j][3]]], m3b[0]);
            glm_vec3_sub(beziator->points[tmp], beziator->points[beziator->surfaces[i][norm_conf[j][4]][norm_conf[j][5]]], m3b[1]);
            glm_vec3_cross(m3b[0], m3b[1], m3b[2]);
            glm_vec3_add(V2[tmp].normal, m3b[2], V2[tmp].normal);
        }

        for (j = 0; j < 4; j++) {
            for (k = 0; k < 4; k++) {
                for (l = 0; l < 4; l++) {
                    bezier((float)j*x, beziator->points[(*surface)[0][l]],
                            beziator->points[(*surface)[1][l]],
                            beziator->points[(*surface)[2][l]],
                            beziator->points[(*surface)[3][l]],
                            u[l][0], u[l][1], u[l][2]);
                    bezier((float)j*x, beziator->points[(*surface)[l][0]],
                            beziator->points[(*surface)[l][1]],
                            beziator->points[(*surface)[l][2]],
                            beziator->points[(*surface)[l][3]],
                            u2[l][0], u2[l][1], u2[l][2]);
                }
                bezier((float)k*x, u[0][0], u[1][0], u[2][0], u[3][0], s[j][k], u[4][1], u[4][2]);
                bezier((float)k*x, u2[0][0], u2[1][0], u2[2][0], u2[3][0], s[j][k], u2[4][1], u2[4][2]);
                glm_vec4_sub(u[4][1], u[4][2], u[4][0]);
                glm_vec4_sub(u2[4][1], u2[4][2], u2[4][0]);
                glm_normalize(u[4][0]);
                glm_normalize(u2[4][0]);
                glm_cross(u[4][0], u2[4][0], norms[j][k]);
                printf("%f %f %f\n%f %f %f\n%f %f %f\n\n",
                        u[4][0][0], u[4][0][1], u[4][0][2],
                        u2[4][0][0], u2[4][0][1], u2[4][0][2],
                        norms[j][k][0], norms[j][k][1], norms[j][k][2]);
                //glm_vec3_copy(u2[4][0], norms[j][k]);
                //glm_vec3_cross(u2[4], u[4], norms[j][k]);
                /*glm_vec3_copy(u[4][1], V3[verts3].position);
                E3[verts3] = verts3;
                verts3++;
                glm_vec3_copy(u[4][2], V3[verts3].position);
                E3[verts3] = verts3;
                verts3++;
                glm_vec3_copy(u2[4][1], V3[verts3].position);
                E3[verts3] = verts3;
                verts3++;
                glm_vec3_copy(u2[4][2], V3[verts3].position);
                E3[verts3] = verts3;
                verts3++;*/
                glm_vec3_copy(s[j][k], V3[verts3].position);
                E3[verts3] = verts3;
                verts3++;
                glm_vec3_normalize(norms[j][k]);
                glm_vec3_add(s[j][k], norms[j][k], V3[verts3].position);
                E3[verts3] = verts3;
                verts3++;
            }
        }
        for (j = 0; j < 3; j++)
            for (k = 0; k < 3; k++) {
                glm_vec3_copy(s[j][k],V[verts].position);
                V[verts].color[1] = 255;
                /*glm_vec3_sub(s[j][k], s[j+1][k], m3b[0]);
                glm_vec3_sub(s[j][k], s[j][k+1], m3b[1]);
                glm_vec3_cross(m3b[0], m3b[1], m3b[2]);*/
                glm_vec3_copy(norms[j][k], V[verts].normal);
                E[verts] = verts;
                verts++;
                glm_vec3_copy(s[j+1][k],V[verts].position);
                V[verts].color[1] = 255;
                //glm_vec3_copy(m3b[2], V[verts].normal);
                glm_vec3_copy(norms[j+1][k], V[verts].normal);
                E[verts] = verts;
                verts++;
                glm_vec3_copy(s[j][k+1],V[verts].position);
                V[verts].color[1] = 255;
                //glm_vec3_copy(m3b[2], V[verts].normal);
                glm_vec3_copy(norms[j][k+1], V[verts].normal);
                E[verts] = verts;
                verts++;
                glm_vec3_copy(s[j+1][k],V[verts].position);
                V[verts].color[1] = 255;
                /*glm_vec3_sub(s[j+1][k+1], s[j][k+1], m3b[0]);
                glm_vec3_sub(s[j+1][k+1], s[j+1][k], m3b[1]);
                glm_vec3_cross(m3b[0], m3b[1], m3b[2]);
                glm_vec3_copy(m3b[2], V[verts].normal);*/
                glm_vec3_copy(norms[j+1][k], V[verts].normal);
                E[verts] = verts;
                verts++;
                glm_vec3_copy(s[j][k+1],V[verts].position);
                V[verts].color[1] = 255;
                //glm_vec3_copy(m3b[2], V[verts].normal);
                glm_vec3_copy(norms[j][k+1], V[verts].normal);
                E[verts] = verts;
                verts++;
                glm_vec3_copy(s[j+1][k+1],V[verts].position);
                V[verts].color[1] = 255;
                glm_vec3_copy(m3b[2], V[verts].normal);
                glm_vec3_copy(norms[j+1][k+1], V[verts].normal);
                E[verts] = verts;
                verts++;

            }
    }
    mesh_update(mesh, sizei, sizei, V, E);
    mesh_update(mesh_skel, sizei, sizei, V2, E2);
    mesh_update(mesh3, sizei, sizei, V3, E3);
    return true;
}
