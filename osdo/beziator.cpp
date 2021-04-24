#include "beziator.h"
#include "conf.h"
#include <cstring>

#define BEZIER_TANGENT_INIT {\
{  0,  0,  0,  0},\
{ -3,  9, -9,  3},\
{  6,-12,  6,  0},\
{ -3,  3,  0,  0},}
#define DETALIZATION 8

#define ucast static_cast<unsigned>

typedef Vertex *surface_t[4][4];

Beziator::Beziator(const string& path) : path(path) {}

bool Beziator::init() {
    printf("%s\n", path.c_str());
    FILE *file = fopen(path.c_str(), "r");
    if (file == nullptr) {
        return false;
    }

    size_t points_size, surfaces_size;
    fscanf(file, "%lu%lu", &points_size, &surfaces_size);
    vector<Vertex> points(points_size);
    vector<GLuint> surfaces_ints(surfaces_size * 16);
    surfacei_t *surfaces = reinterpret_cast<surfacei_t*>(surfaces_ints.data());

    points_size = points.size();
    surfaces_size = surfaces_ints.size() / 16;
    for (size_t i = 0; i < points_size; i++) {
        vec4 init = GLM_VEC4_BLACK_INIT;
        vec4 &point = points[i].position;
        glm_vec4_copy(init, point);
        fscanf(file, "%f%f%f", point, point + 1, point + 2);
    }
    int j, k;
    for (size_t i = 0; i < surfaces_size; i++) {
        for (j = 0; j < 4; j++)
            for (k = 0; k < 4; k++) {
                fscanf(file, "%u", surfaces[i][j] + k);
            }
    }
    fclose(file);
    update(points, surfaces_ints);
    return true;
}

Beziator::~Beziator() {
}

void Beziator::draw(Shader &shader) {
    //shader_set_vec3(this->editmode, "min_coord", this->min_coord);
    //shader_set_vec3(this->editmode, "max_coord", this->max_coord);
    //mesh_draw_mode(&this->frame, GL_POINTS);
    //mesh_draw_mode(&this->frame, GL_LINES);
    shader.set_float("alpha", 0.5f);
    this->mesh.draw_mode(GL_TRIANGLES);
    //Mesh::draw_mode(GL_TRIANGLES);
    //mesh_draw_mode(&this->normals, GL_LINES);
    shader.set_float("alpha", 1);
}


void bezier_curve(float a, mat4 points, vec4 dest) {
    mat4 matrix;
    glm_vec4_cubic(a, dest);
    glm_mat4_mul(points, mat4 GLM_BEZIER_MAT_INIT, matrix);
    glm_mat4_mulv(matrix, dest, dest);
}

void bezier_curve_tangent(float a, mat4 points, vec4 dest) {
    mat4 matrix;
    glm_vec4_cubic(a, dest);
    glm_mat4_mul(points, mat4 BEZIER_TANGENT_INIT, matrix);
    glm_mat4_mulv(matrix, dest, dest);
}

void bezier_surface(
        float u, float v, surface_t points, vec4 dest, vec4 normal) {
    mat4 m, res1, res2, res3;

    for (int i = 0; i < 4; i++) {
        glm_vec4_copy(points[0][i]->position, m[0]);
        glm_vec4_copy(points[1][i]->position, m[1]);
        glm_vec4_copy(points[2][i]->position, m[2]);
        glm_vec4_copy(points[3][i]->position, m[3]);
        bezier_curve(u, m, res1[i]);

        glm_vec4_copy(points[i][0]->position, m[0]);
        glm_vec4_copy(points[i][1]->position, m[1]);
        glm_vec4_copy(points[i][2]->position, m[2]);
        glm_vec4_copy(points[i][3]->position, m[3]);
        bezier_curve(v, m, res2[i]);
    }

    bezier_curve(v, res1, dest);
    bezier_curve_tangent(v, res1, res3[1]);
    bezier_curve_tangent(u, res2, res3[3]);

    glm_cross(res3[1], res3[3], normal);
}

bool Beziator::save() {
    FILE *file = fopen(this->path.c_str(), "w");
    if (file == nullptr) {
        printf("ERROR: failed to open file %s\n", this->path.c_str());
        return false;
    }
    size_t surfaces_size = this->indices.size() / 16;
    surfacei_t *surfaces = reinterpret_cast<surfacei_t*>(indices.data());
    fprintf(file, "%lu %lu\n", this->vertices.size(), this->indices.size() / 16);
    for (size_t i = 0; i < this->vertices.size(); i++) {
        vec4 &point = this->vertices[i].position;
        fprintf(file, "%f %f %f\n", static_cast<double>(point[0]),
                static_cast<double>(point[1]), static_cast<double>(point[2]));
    }
    int j, k;
    for (size_t i = 0; i < surfaces_size; i++) {
        for (j = 0; j < 4; j++)
            for (k = 0; k < 4; k++) {
                fprintf(file, "%u ", surfaces[i][j][k]);
            }
        fprintf(file, "\n");
    }
    fclose(file);
    return true;
}

void Beziator::generate() {
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
    surface_t surface;
    GLuint verts = 0, verts2 = 0;
    //verts3 = 0;
    const int *c;
    mat4 m4b;
    uint8_t si, sj;
    const uint8_t (*st)[2];

    Mesh *mesh = &this->mesh, *mesh_skel = &this->frame;
            //*mesh_normals = &this->normals;
    const size_t d = DETALIZATION;
    x = 1.f / (d - 1);

    const size_t surfaces_size = this->indices.size() / 16;
    surfacei_t *surfaces = reinterpret_cast<surfacei_t*>(indices.data());
    const size_t size = 6 * 9 * d * d * surfaces_size;
    //const GLsizei sizei = static_cast<GLsizei>(size);
    vector<Vertex> V(size);
    vector<GLuint> E(size);
    vector<Vertex> V2(size);
    vector<GLuint> E2(size);
    vector<Vertex> V3(this->vertices.size());
    vector<GLuint> E3(this->vertices.size() * 4);

    // Creator frame vertices
    for (size_t i = 0; i < this->vertices.size(); i++) {
        point = &this->vertices[i].position;
        glm_vec3_copy(*point, V2[i].position);
        V2[i].color[1] = 255;
        V2[i].color[3] = 255;
    }

    for (size_t i = 0; i < surfaces_size; i++) {
        for (j = 0; j < 4; j++) {
            for (k = 0; k < 4; k++) {
                surface[j][k] = &(this->vertices[surfaces[i][j][k]]);
            }
        }
        // Creator frame lines
        for (j = 0; j < ctrls_size; j++) {
            c = controls_lines[j];
            E2[verts2++] = ucast(surfaces[i][c[0]][c[1]]);
        }

        // Create vertices
        for (j = 0; j < d; j++) {
            for (k = 0; k < d; k++) {
                u = static_cast<float>(j)*x; v = static_cast<float>(k)*x;
                index = i * d * d + j * d + k;
                bezier_surface(u, v, surface, vertex, normal);
                glm_normalize(normal);
                glm_vec3_copy(vertex, V[index].position);
                glm_vec3_copy(normal, V[index].normal);
                V[verts].color[1] = 255;
                /*glm_vec3_copy(vertex, V3[verts3].position);
                E3[verts3] = verts3;
                verts3++;
                glm_vec3_add(normal, vertex, V3[verts3].position);
                E3[verts3] = verts3;
                verts3++;*/
            }
        }

        // Create triangles
        for (j = 0; j < d - 1; j++)
            for (k = 0; k < d - 1; k++) {
                E[verts++] = ucast(i * d * d + (j + 1) * d + k);
                E[verts++] = ucast(i * d * d + j * d + k);
                E[verts++] = ucast(i * d * d + j * d + k + 1);

                E[verts++] = ucast(i * d * d + j * d + k + 1);
                E[verts++] = ucast(i * d * d + (j + 1) * d + k + 1);
                E[verts++] = ucast(i * d * d + (j + 1) * d + k);
            }

        for (si = 0; si < 3; si++) {
            for (sj = 0; sj < 3; sj++) {
                st = SQUARE_TYPES[BEZIER_SQUARE_TYPES[si][sj]];
                while (st[2][0] != 9) {
                    if (st[2][0] == 8) {
                        st += 3;
                    }
                    index = static_cast<size_t>((surface)[si+st[1][0]][sj+st[1][1]] - this->vertices.data());
                    glm_vec3_sub(((surface)[si+st[1][0]][sj+st[1][1]])->position,
                            ((surface)[si+st[0][0]][sj+st[0][1]])->position, m4b[0]);
                    glm_vec3_sub(((surface)[si+st[1][0]][sj+st[1][1]])->position,
                            (surface[si+st[2][0]][sj+st[2][1]])->position, m4b[1]);
                    glm_vec3_cross(m4b[0], m4b[1], m4b[2]);
                    glm_vec3_add(V2[index].normal, m4b[2], V2[index].normal);
                    st++;
                }
            }
        }
    }
    /*
    // Example drawing of normals for frame
    for (size_t i = 0; i < this->points_size; i++) {
        glm_normalize(V2[i].normal);
        glm_vec3_add(V2[i].position, V2[i].normal,
                     V2[i+this->points_size].position);
        E2[verts2++] = (unsigned)i;
        E2[verts2++] = (unsigned)(i+this->points_size);
    }*/
    mesh->update(V, E);
    mesh_skel->update(V2, E2);
    //mesh_update(mesh_normals, sizei, sizei, V3, E3);
}

void Beziator::rotate(size_t i) {
    surfacei_t s;
    surfacei_t *surfaces = reinterpret_cast<surfacei_t*>(indices.data());
    memcpy(s, surfaces[i], sizeof(surfacei_t));
    for (int k = 0; k < 4; k++)
        for (int j = 0; j < 4; j++) {
            surfaces[i][k][j] = s[j][k];
        }
}

vector<Vertex> &Beziator::get_vertices() {
    return vertices;
}
