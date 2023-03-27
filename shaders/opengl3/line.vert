#version 400 core
layout (location = 0) in vec3 Position;
layout (location = 1) in vec3 VertColor;

layout (std140) uniform Matrices
{
    mat4 projection;
    mat4 view;
};

out vec3 color;

void main()
{
    gl_Position = projection * view * vec4(Position, 1.0);
	//gl_Position = vec4(Position, 1.0);
	color = VertColor;
}