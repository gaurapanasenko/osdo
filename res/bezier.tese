#version 400 core

layout(quads, equal_spacing) in;

struct Data {
    vec4 color;
    vec2 uv;
    vec3 pos;
    vec3 normal;
};

in Data tesc_data[];
out Data geometry;

void main(void)
{
    float   x = gl_TessCoord.x;
    float   y = gl_TessCoord.y;
    vec4    u = vec4 ( 1.0, x, x*x, x*x*x );
    vec4    v = vec4 ( 1.0, y, y*y, y*y*y );
    vec4    uu = vec4 ( 0, 1.0, 2*x, 3*x*x);
    vec4    vv = vec4 ( 0, 1.0, 2*y, 3*y*y);
    mat4    b = mat4 (  1,  0,  0, 0,
                       -3,  3,  0, 0,
                        3, -6,  3, 0,
                       -1,  3, -3, 1 );
   mat4    db = mat4 (-3,  3,  0,  0,
                       6,-12,  6,  0,
                      -3,  9, -9,  3,
                       0,  0,  0,  0);

    vec4    bu = b * u;
    vec4    bv = b * v;
    vec4    buu = db * u;
    vec4    bvv = db * v;

    vec4 p00 = gl_in [ 0].gl_Position;
    vec4 p10 = gl_in [ 1].gl_Position;
    vec4 p20 = gl_in [ 2].gl_Position;
    vec4 p30 = gl_in [ 3].gl_Position;
    vec4 p01 = gl_in [ 4].gl_Position;
    vec4 p11 = gl_in [ 5].gl_Position;
    vec4 p21 = gl_in [ 6].gl_Position;
    vec4 p31 = gl_in [ 7].gl_Position;
    vec4 p02 = gl_in [ 8].gl_Position;
    vec4 p12 = gl_in [ 9].gl_Position;
    vec4 p22 = gl_in [10].gl_Position;
    vec4 p32 = gl_in [11].gl_Position;
    vec4 p03 = gl_in [12].gl_Position;
    vec4 p13 = gl_in [13].gl_Position;
    vec4 p23 = gl_in [14].gl_Position;
    vec4 p33 = gl_in [15].gl_Position;

    gl_Position = bu.x * (bv.x * p00 + bv.y * p01 + bv.z * p02 + bv.w * p03) +
                  bu.y * (bv.x * p10 + bv.y * p11 + bv.z * p12 + bv.w * p13) +
                  bu.z * (bv.x * p20 + bv.y * p21 + bv.z * p22 + bv.w * p23) +
                  bu.w * (bv.x * p30 + bv.y * p31 + bv.z * p32 + bv.w * p33);
    vec4 du = buu.x * (bv.x * p00 + bv.y * p01 + bv.z * p02 + bv.w * p03) +
                  buu.y * (bv.x * p10 + bv.y * p11 + bv.z * p12 + bv.w * p13) +
                  buu.z * (bv.x * p20 + bv.y * p21 + bv.z * p22 + bv.w * p23) +
                  buu.w * (bv.x * p30 + bv.y * p31 + bv.z * p32 + bv.w * p33);
    vec4 dv = bvv.x * (bu.x * p00 + bu.y * p01 + bu.z * p02 + bu.w * p03) +
                bvv.y * (bu.x * p10 + bu.y * p11 + bu.z * p12 + bu.w * p13) +
                bvv.z * (bu.x * p20 + bu.y * p21 + bu.z * p22 + bu.w * p23) +
                bvv.w * (bu.x * p30 + bu.y * p31 + bu.z * p32 + bu.w * p33);
    geometry.color = tesc_data[0].color;
    geometry.uv = tesc_data[0].uv;
    geometry.pos = tesc_data[0].pos;
    geometry.normal = vec3(cross(vec3(du), vec3(dv)));
}
