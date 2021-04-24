#version 330
layout(triangles) in;
layout(triangle_strip, max_vertices=3) out;

struct Data {
    vec4 color;
    vec2 uv;
    vec3 pos;
    vec3 normal;
};

in Data vertex[3];
out Data geometry;

void main() {
    int i;
    for(i = 0; i < 3; i++) {
        gl_Position = gl_in[i].gl_Position;
        geometry.color = vertex[i].color;
        geometry.uv = vertex[i].uv;
        geometry.pos = vertex[i].pos;
        geometry.normal = vertex[i].normal;
        EmitVertex();
    }
    EndPrimitive();
}
