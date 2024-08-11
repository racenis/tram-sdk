#version 300 es

precision highp float;

layout (location = 0) in vec3 Position;
layout (location = 1) in vec3 Normal;
layout (location = 2) in vec2 VertUV;
layout (location = 3) in vec2 VertLightUV;
layout (location = 4) in uint TexIndex;

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
	vec4 ambientcolor;
	float time;
	float sunweight;
	float screenwidth;
	float screenheight;
};

out vec2 vertUV;
out vec2 vertLightUV;
out vec3 vertColor;
flat out uint texIndex;




void main()
{
    gl_Position = projection * view * model * vec4(Position, 1.0);
	vec3 nPos = normalize(vec3(model * vec4(Normal, 0.0)));
	vec3 lightColor = vec3(ambientcolor);
	lightColor += suncolor * max(dot(nPos, normalize(sundir)), 0.0);
	lightColor *= sunweight;
	
    vertUV = VertUV;
	texIndex = TexIndex;
	vertLightUV = VertLightUV;
	vertColor = lightColor + (1.0 - sunweight);
}