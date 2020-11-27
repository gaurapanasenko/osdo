#version 330 core
out vec4 FragColor;

in vec4 frag_color;
in vec2 frag_uv;

uniform sampler2D frag_texture;

void main()
{
    FragColor = frag_color * texture(frag_texture, frag_uv.st);
}
