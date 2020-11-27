#version 330 core
precision mediump float;
layout (location = 0) in vec2 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec4 color;
layout (location = 3) in vec2 uv;

out vec4 frag_color;
out vec2 frag_uv;

uniform mat4 projection;

void main()
{
    gl_Position = projection * vec4(position.xy, 0, 1.0);
    frag_color = color;
    frag_uv = uv;
}
