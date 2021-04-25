#version 420 core

layout(quads, equal_spacing) in;

struct Data {
    vec4 color;
    vec2 uv;
    vec3 normal;
    vec3 frag_pos;
};

layout(location = 0) in Data inData[];
layout(location = 0) out Data outData;

mat4  b = mat4 ( 1,  0,  0, 0,
                -3,  3,  0, 0,
                 3, -6,  3, 0,
                -1,  3, -3, 1);

void main(void) {
    float x = gl_TessCoord.x;
    float y = gl_TessCoord.y;
    vec4 u = vec4 (1.0, x, x*x, x*x*x);
    vec4 v = vec4 (1.0, y, y*y, y*y*y);
    vec4 uu = vec4 (0, 1.0, 2*x, 3*x*x);
    vec4 vv = vec4 (0, 1.0, 2*y, 3*y*y);

    vec4 bu = b * u;
    vec4 bv = b * v;
    vec4 buu = b * uu;
    vec4 bvv = b * vv;

    mat4 pu[4], pv[4], cu, cv;
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            pv[i][j] = gl_in[j*4 + i].gl_Position;
        }
    }
    for (int i = 0; i < 4; i++) {
        cv[i] = pv[i] * bv;
    }

    gl_Position = cv * bu;

    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            pu[i][j] = vec4(inData[i*4 + j].normal, 1);
            pv[i][j] = vec4(inData[j*4 + i].normal, 1);
        }
    }
    for (int i = 0; i < 4; i++) {
        cu[i] = pu[i] * bu;
        cv[i] = pv[i] * bv;
    }
    vec4 du = cv * buu, dv = cu * bvv;
    outData.normal = cross(vec3(du), vec3(dv));

    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            pv[i][j] = vec4(inData[j*4 + i].frag_pos, 1);
        }
    }
    for (int i = 0; i < 4; i++) {
        cv[i] = pv[i] * bv;
    }
    outData.frag_pos = vec3(cv * bu);


    /*for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            pv[i][j] = vec4(inData[i*4 + i].uv, 0, 1);
        }
    }
    for (int i = 0; i < 4; i++) {
        cv[i] = pv[i] * bv;
    }
    outData.uv = vec2(cv * bu);*/
    outData.uv = vec2(x, y);

    outData.color = inData[0].color;
}
