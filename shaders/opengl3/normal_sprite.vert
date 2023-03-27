#version 400 core
layout (location = 0) in vec3 Position;
layout (location = 3) in vec2 VOffset;
layout (location = 1) in vec2 VertUV;
layout (location = 4) in float Verticality;
layout (location = 5) in uint TexIndex;

layout (std140) uniform Matrices
{
    mat4 projection;
    mat4 view;
};

layout (std140) uniform ModelMatrices
{
    mat4 model;
	uvec4 modellights;
	vec3 sundir;
	vec3 suncolor;
	vec3 ambientcolor;
	vec3 time;
};

out vec2 vertUV;
flat out uint texIndex;




void main()
{
	vec4 scr_pos = projection * view * model * vec4(Position, 1.0);
	
    gl_Position = scr_pos + vec4(VOffset, 0.0, 0.0);

    vertUV = VertUV;
	texIndex = TexIndex;
}