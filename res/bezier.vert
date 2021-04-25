#version 420 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec4 color;
layout (location = 3) in vec2 uv;

struct Data {
    vec4 color;
    vec2 uv;
    vec3 normal;
    vec3 frag_pos;
};

layout(location = 0) out Data data;

uniform mat4 model;
uniform mat4 camera;
uniform mat4 projection;

void main()
{
    mat4 trans = projection * camera * model;
    vec4 pos = trans * vec4(position, 1.0);
    gl_Position = pos;
    data.color = color;
    data.uv = uv;
    data.frag_pos = vec3(model * vec4(position, 1.0));
    data.normal = mat3(transpose(inverse(model))) * vec3(normal);
}
