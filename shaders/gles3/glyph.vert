#version 300 es

precision highp float;

layout (location = 0) in vec2 Position;
layout (location = 1) in vec2 TexCoord;
layout (location = 2) in vec3 VertColor;
layout (location = 5) in uint Texture;

out vec2 vertUV;
out vec3 color;
flat out uint texindex;

layout (std140) uniform ModelMatrices
{
    mat4 model;
	uvec4 modellights;
	vec3 sundir;
	vec3 suncolor;
	vec4 ambientcolor;
	float time;
	float sunweight;
	float screenwidth;
	float screenheight;
};

void main()
{
    gl_Position = vec4((Position.x / (screenwidth / 2.0)) - 1.0, (Position.y / (screenheight / -2.0)) + 1.0, -0.5, 1.0);
	color = VertColor;
	vertUV = vec2(TexCoord.x / 256.0, TexCoord.y / -256.0);
	texindex = Texture;
}