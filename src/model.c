#include "model.h"
#include "conf.h"

#define model_PATH RES_DIR"/%s.odom"

bool model_init(Model *model, const char *name) {
    mesh_subinit(&model->mesh, name);
    mesh_subinit(&model->frame, name);
    const size_t path_len = strlen(model_PATH);
    const size_t len = strlen(name);
    strcpy(model->name, name);
    char *path = calloc(len + path_len, sizeof(char));
    snprintf(path, len + path_len, model_PATH, name);
    FILE *file = fopen(path, "r");
    if (file == NULL) {
        printf("ERROR: failed to open file %s\n", path);
        free(path);
        return false;
    }
    free(path);

    fscanf(file, "%lu%lu", &model->points_size, &model->surfaces_size);
    model->points = (vec4*)calloc(model->points_size, sizeof(vec4));
    if (model->points == NULL) {
        fclose(file);
        printf("Failed to allocate memory.\n");
        return false;
    }
    model->surfaces = (surface_t*)calloc(model->surfaces_size, sizeof(surface_t));
    if (model->points == NULL) {
        fclose(file);
        free(model->points);
        printf("Failed to allocate memory.\n");
        return false;
    }
    vec4 *point;
    surface_t *surface;
    for (size_t i = 0; i < model->points_size; i++) {
        point = &model->points[i];
        glm_vec4_copy(GLM_VEC4_BLACK, *point);
        fscanf(file, "%f%f%f", (*point), (*point) + 1, (*point) + 2);
    }
    int j, k;
    for (size_t i = 0; i < model->surfaces_size; i++) {
        surface = &model->surfaces[i];
        for (j = 0; j < 4; j++)
            for (k = 0; k < 4; k++)
                fscanf(file, "%lu", (*surface)[j] + k);
    }
    fclose(file);
    return true;
}

void model_del(Model *model) {
    if (model->points) {
        free(model->points);
        model->points = NULL;
    }
    if (model->surfaces) {
        free(model->surfaces);
        model->surfaces = NULL;
    }
    mesh_del(&model->mesh);
    mesh_del(&model->frame);
}

void model_draw(Model *model) {
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    mesh_draw(&model->frame);
    //glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    mesh_draw(&model->mesh);
}

void part(float a, vec4 p1, vec4 p2, vec4 dest) {
    glm_vec4_copy(GLM_VEC4_BLACK, dest);
    glm_vec4_muladds(p1, a, dest);
    glm_vec4_muladds(p2, 1.f - a, dest); //p1*a + p2*(1-a)
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

bool model_generate(Model *model) {
    Mesh *mesh = &model->mesh, *mesh_skel = &model->frame;
    //const size_t size_name = strlen(model->name);
    strcpy(model->name, mesh->name);
    //strcpy(model->name, mesh_skel->name);
    //strcpy(mesh_skel->name + size_name, "_wire");
    const unsigned long size = 6 * 9 * 16 * model->surfaces_size;
    const GLsizei sizei = (GLsizei)size;
    Vertex *V = (Vertex*)calloc(size, sizeof(Vertex));
    GLuint *E = (GLuint*)calloc(size, sizeof(GLuint));
    Vertex *V2 = (Vertex*)calloc(size, sizeof(Vertex));
    GLuint *E2 = (GLuint*)calloc(size, sizeof(GLuint));
    int j, k, l;
    GLuint verts = 0, verts2 = 0;
    vec4 s[8][8], v[4]; float x = 1.f/7;
    for (size_t i = 0; i < model->surfaces_size; i++) {
        for (j = 0; j < 8; j++) {
            for (k = 0; k < 8; k++) {
                for (l = 0; l < 4; l++) {
                    bezier((float)j*x, model->points[model->surfaces[i][0][l]],
                            model->points[model->surfaces[i][1][l]],
                            model->points[model->surfaces[i][2][l]],
                            model->points[model->surfaces[i][3][l]],
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

        glm_vec3_copy(model->points[model->surfaces[i][j][k]],V2[verts2].position);
        V2[verts2].color[1] = model->points[model->surfaces[i][j][k]][2] * 100;
        E2[verts2] = verts2;
        verts2++;
        glm_vec3_copy(model->points[model->surfaces[i][j+1][k]],V2[verts2].position);
        V2[verts2].color[1] = model->points[model->surfaces[i][j+1][k]][2] * 100;
        E2[verts2] = verts2;
        verts2++;
        glm_vec3_copy(model->points[model->surfaces[i][j][k+1]],V2[verts2].position);
        V2[verts2].color[1] = model->points[model->surfaces[i][j][k+1]][2] * 100;
        E2[verts2] = verts2;
        verts2++;
        glm_vec3_copy(model->points[model->surfaces[i][j+1][k]],V2[verts2].position);
        V2[verts2].color[1] = model->points[model->surfaces[i][j+1][k]][2] * 100;
        E2[verts2] = verts2;
        verts2++;
        glm_vec3_copy(model->points[model->surfaces[i][j][k+1]],V2[verts2].position);
        V2[verts2].color[1] = model->points[model->surfaces[i][j][k+1]][2] * 100;
        E2[verts2] = verts2;
        verts2++;
        glm_vec3_copy(model->points[model->surfaces[i][j+1][k+1]],V2[verts2].position);
        V2[verts2].color[1] = model->points[model->surfaces[i][j+1][k+1]][2] * 100;
        E2[verts2] = verts2;
        verts2++;
            }
    }
    mesh_update(mesh, sizei, sizei, V, E);
    mesh_update(mesh_skel, sizei, sizei, V2, E2);
    return true;
}
