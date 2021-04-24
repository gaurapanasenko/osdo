#version 400 core

struct Data {
    vec4 color;
    vec2 uv;
    vec3 pos;
    vec3 normal;
};

in Data vertex[];
out Data tesc_data[];

uniform int inner;
uniform int outer;

layout(vertices = 16) out;

void main(void) {
    gl_TessLevelInner[0] = inner;
    gl_TessLevelInner[1] = inner;
    gl_TessLevelOuter[0] = outer;
    gl_TessLevelOuter[1] = outer;
    gl_TessLevelOuter[2] = outer;
    gl_TessLevelOuter[3] = outer;

    gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;
    tesc_data[gl_InvocationID].color = vertex[gl_InvocationID].color;
    tesc_data[gl_InvocationID].uv = vertex[gl_InvocationID].uv;
    tesc_data[gl_InvocationID].pos = vertex[gl_InvocationID].pos;
    tesc_data[gl_InvocationID].normal = vertex[gl_InvocationID].normal;
}
