#version 420 core

struct Data {
    vec4 color;
    vec2 uv;
    vec3 normal;
    vec3 frag_pos;
};

layout(location = 0) in Data inData[];
layout(location = 0) out Data outData[];

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
    outData[gl_InvocationID].color = inData[gl_InvocationID].color;
    outData[gl_InvocationID].uv = inData[gl_InvocationID].uv;
    outData[gl_InvocationID].normal = inData[gl_InvocationID].normal;
    outData[gl_InvocationID].frag_pos = inData[gl_InvocationID].frag_pos;
}
