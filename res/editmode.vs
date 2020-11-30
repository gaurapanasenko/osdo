#version 330 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec4 color;
layout (location = 3) in vec2 uv;

out vec4 frag_color;
out vec2 frag_uv;
out vec4 frag_pos;
out vec3 frag_min_coord;
out vec3 frag_max_coord;
out vec2 viewPortSize;

uniform mat4 model;
uniform mat4 camera;
uniform mat4 projection;
uniform vec3 min_coord;
uniform vec3 max_coord;
uniform vec2 vp;

void main()
{
    mat4 trans = projection * camera * model;
    vec4 pos = trans * vec4(position, 1.0);
    gl_Position = pos;
    frag_color = color;
    frag_uv = uv;
    frag_pos = pos;
    frag_min_coord = vec3(trans * vec4(min_coord, 1.0));
    frag_max_coord = vec3(trans * vec4(max_coord, 1.0));
    viewPortSize = vp;
}
