#version 330 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec4 color;
layout (location = 3) in vec2 uv;

out vec4 frag_color;
out vec2 frag_uv;

uniform mat4 model;
uniform mat4 camera;
uniform mat4 projection;

void main()
{
    gl_Position = projection * camera * model * vec4(position, 1.0);
    frag_color = color;
    frag_uv = uv;
}
