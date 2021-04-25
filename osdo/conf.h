#ifndef CONF_H
#define CONF_H

#include "osdo.h"
#include "mesh.h"

#define M_PI 3.14159265358979323846
#define M_RAD M_PI / 180
#define M_PI_F 3.14159265358979323846f
#define M_RAD_F M_PI_F / 180

#define RES_DIR "../share/osdo"

#define VERTEX_PATH RES_DIR"/%s.vert"
#define TESC_PATH RES_DIR"/%s.tesc"
#define TESE_PATH RES_DIR"/%s.tese"
#define GEOMETRY_PATH RES_DIR"/%s.geom"
#define FRAGMENT_PATH RES_DIR"/%s.frag"
#define BEZIATOR_PATH RES_DIR"/%s.odom"

#define MAX_VERTEX_BUFFER 512 * 1024
#define MAX_ELEMENT_BUFFER 128 * 1024

#define NK_GLFW_DOUBLE_CLICK_LO 0.02
#define NK_GLFW_DOUBLE_CLICK_HI 0.2

#define BASIS0POS (vec3{ 0.0f, 0.0f,-32.0f})
#define BASIS1POS (vec3{-8.0f, 0.0f,  0.0f})
#define BASIS2POS (vec3{ 8.0f, 0.0f,  0.0f})

#define BASIS1ROT (vec3{ 0.0f, 0.0f,  0.2f})
#define BASIS2ROT (vec3{ 0.0f, 0.0f, -0.2f})

static const unsigned int SCR_WIDTH = 1366;
static const unsigned int SCR_HEIGHT = 700;

static const float OBJECT_MOVE_SPEED   =  5.0f;
static const float OBJECT_ROTATE_SPEED =  1.0f;
static const float OBJECT_ANIMATE_SPEED = 1.0f;

static const float SENSITIVITY = 0.01f;

static vec3 UNUSED LAMP_POSITIONS[] = {
    {5.0f, 0.0f, 5.0f},
    {-1.0f, 0.0f, 1.0f}
};

static const Vertex EXAMPLE_CUBE_VERTEX[] = {
    {VERTEX_SIMPLE, 0, {-1.,  1., -1.}, { 0.,  1.,  0.}, {  0, 255,   0, 255}, {0., 0.}},
    {VERTEX_SIMPLE, 0, { 1.,  1.,  1.}, { 0.,  1.,  0.}, {255, 255, 255, 255}, {0., 0.}},
    {VERTEX_SIMPLE, 0, { 1.,  1., -1.}, { 0.,  1.,  0.}, {255, 255,   0, 255}, {0., 0.}},

    {VERTEX_SIMPLE, 0, { 1.,  1.,  1.}, { 0.,  0.,  1.}, {255, 255, 255, 255}, {0., 0.}},
    {VERTEX_SIMPLE, 0, {-1., -1.,  1.}, { 0.,  0.,  1.}, {  0,   0, 255, 255}, {0., 0.}},
    {VERTEX_SIMPLE, 0, { 1., -1.,  1.}, { 0.,  0.,  1.}, {255,   0, 255, 255}, {0., 0.}},

    {VERTEX_SIMPLE, 0, {-1.,  1.,  1.}, {-1.,  0.,  0.}, {  0, 255, 255, 255}, {0., 0.}},
    {VERTEX_SIMPLE, 0, {-1., -1., -1.}, {-1.,  0.,  0.}, {  0,   0,   0, 255}, {0., 0.}},
    {VERTEX_SIMPLE, 0, {-1., -1.,  1.}, {-1.,  0.,  0.}, {  0,   0, 255, 255}, {0., 0.}},

    {VERTEX_SIMPLE, 0, { 1., -1., -1.}, { 0., -1.,  0.}, {255,   0,   0, 255}, {0., 0.}},
    {VERTEX_SIMPLE, 0, {-1., -1.,  1.}, { 0., -1.,  0.}, {  0,   0, 255, 255}, {0., 0.}},
    {VERTEX_SIMPLE, 0, {-1., -1., -1.}, { 0., -1.,  0.}, {  0,   0,   0, 255}, {0., 0.}},

    {VERTEX_SIMPLE, 0, { 1.,  1., -1.}, { 1.,  0.,  0.}, {255, 255,   0, 255}, {0., 0.}},
    {VERTEX_SIMPLE, 0, { 1., -1.,  1.}, { 1.,  0.,  0.}, {255,   0, 255, 255}, {0., 0.}},
    {VERTEX_SIMPLE, 0, { 1., -1., -1.}, { 1.,  0.,  0.}, {255,   0,   0, 255}, {0., 0.}},

    {VERTEX_SIMPLE, 0, {-1.,  1., -1.}, { 0.,  0., -1.}, {  0, 255,   0, 255}, {0., 0.}},
    {VERTEX_SIMPLE, 0, { 1., -1., -1.}, { 0.,  0., -1.}, {255,   0,   0, 255}, {0., 0.}},
    {VERTEX_SIMPLE, 0, {-1., -1., -1.}, { 0.,  0., -1.}, {  0,   0,   0, 255}, {0., 0.}},

    {VERTEX_SIMPLE, 0, {-1.,  1., -1.}, { 0.,  1.,  0.}, {  0, 255,   0, 255}, {0., 0.}},
    {VERTEX_SIMPLE, 0, {-1.,  1.,  1.}, { 0.,  1.,  0.}, {  0, 255, 255, 255}, {0., 0.}},
    {VERTEX_SIMPLE, 0, { 1.,  1.,  1.}, { 0.,  1.,  0.}, {255, 255, 255, 255}, {0., 0.}},

    {VERTEX_SIMPLE, 0, { 1.,  1.,  1.}, { 0.,  0.,  1.}, {255, 255, 255, 255}, {0., 0.}},
    {VERTEX_SIMPLE, 0, {-1.,  1.,  1.}, { 0.,  0.,  1.}, {  0, 255, 255, 255}, {0., 0.}},
    {VERTEX_SIMPLE, 0, {-1., -1.,  1.}, { 0.,  0.,  1.}, {  0,   0, 255, 255}, {0., 0.}},

    {VERTEX_SIMPLE, 0, {-1.,  1.,  1.}, {-1.,  0.,  0.}, {  0, 255, 255, 255}, {0., 0.}},
    {VERTEX_SIMPLE, 0, {-1.,  1., -1.}, {-1.,  0.,  0.}, {  0, 255,   0, 255}, {0., 0.}},
    {VERTEX_SIMPLE, 0, {-1., -1., -1.}, {-1.,  0.,  0.}, {  0,   0,   0, 255}, {0., 0.}},

    {VERTEX_SIMPLE, 0, { 1., -1., -1.}, { 0., -1.,  0.}, {255,   0,   0, 255}, {0., 0.}},
    {VERTEX_SIMPLE, 0, { 1., -1.,  1.}, { 0., -1.,  0.}, {255,   0, 255, 255}, {0., 0.}},
    {VERTEX_SIMPLE, 0, {-1., -1.,  1.}, { 0., -1.,  0.}, {  0,   0, 255, 255}, {0., 0.}},

    {VERTEX_SIMPLE, 0, { 1.,  1., -1.}, { 1.,  0.,  0.}, {255, 255,   0, 255}, {0., 0.}},
    {VERTEX_SIMPLE, 0, { 1.,  1.,  1.}, { 1.,  0.,  0.}, {255, 255, 255, 255}, {0., 0.}},
    {VERTEX_SIMPLE, 0, { 1., -1.,  1.}, { 1.,  0.,  0.}, {255,   0, 255, 255}, {0., 0.}},

    {VERTEX_SIMPLE, 0, {-1.,  1., -1.}, { 0.,  0., -1.}, {  0, 255,   0, 255}, {0., 0.}},
    {VERTEX_SIMPLE, 0, { 1.,  1., -1.}, { 0.,  0., -1.}, {255, 255,   0, 255}, {0., 0.}},
    {VERTEX_SIMPLE, 0, { 1., -1., -1.}, { 0.,  0., -1.}, {255,   0,   0, 255}, {0., 0.}},
};

static const GLuint EXAMPLE_CUBE_INDICIES[] = {
     0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11,
    12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23,
    24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35,
};

#endif // CONF_H