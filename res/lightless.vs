#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 2) in vec3 aColor;

out vec3 ourColor;

uniform mat4 model;
uniform mat4 camera;
uniform mat4 projection;

void main()
{
        gl_Position = projection * camera * model * vec4(aPos, 1.0);
	ourColor = aColor;
}
