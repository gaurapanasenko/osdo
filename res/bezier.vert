#version 400 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec4 color;
layout (location = 3) in vec2 uv;

struct Data {
    vec4 color;
    vec2 uv;
    vec3 pos;
    vec3 normal;
};

out Data vertex;

uniform mat4 model;
uniform mat4 camera;
uniform mat4 projection;
uniform vec3 min_coord;
uniform vec3 max_coord;

void main()
{
    mat4 trans = projection * camera * model;
    vec4 pos = trans * vec4(position, 1.0);
    gl_Position = pos;
    vertex.color = color;
    vertex.uv = uv;
    vertex.pos = vec3(model * vec4(position, 1.0));
    vertex.normal = mat3(transpose(inverse(model))) * normal;
}
