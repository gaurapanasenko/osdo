#version 330 core
out vec4 FragColor;

in vec4 frag_color;
in vec2 frag_uv;

void main()
{
    FragColor = frag_color;
}
