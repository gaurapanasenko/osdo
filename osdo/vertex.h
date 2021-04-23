#ifndef VERTEX_H
#define VERTEX_H
#include "osdo.h"

enum VetrexType {
    VERTEX_SIMPLE = 0,
    VERTEX_BEZIER = 1,
};

struct Vertex {
    VetrexType type;
    int node_id;
    vec4 position;
    vec3 normal;
    unsigned char color[4];
    vec2 uv;
    /*Vertex(VetrexType type, int node_id, vec3 position,
           vec3 normal, unsigned char color[4], vec2 uv)
        : type(type), node_id(node_id)
    {}*/
};

#endif // VERTEX_H
