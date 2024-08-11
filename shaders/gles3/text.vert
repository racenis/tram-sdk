#version 300 es

// idk if this shader is even used anymore

// probably not

precision highp float;

layout (location = 0) in vec2 Position;
layout (location = 1) in vec2 TexCoord;
layout (location = 2) in vec3 VertColor;
layout (location = 3) in vec2 Scale;
layout (location = 4) in float Thickness;
layout (location = 5) in uint Texture;

out vec2 vertUV;
out vec3 color;
out vec2 cscale;
out float thick;
flat out uint texindex;

void main()
{
    gl_Position = vec4((Position.x / 320.0) - 1.0, (Position.y / -240.0) + 1.0, -0.5, 1.0);
	color = VertColor;
	vertUV = vec2(TexCoord.x / 256.0, TexCoord.y / -256.0);
	cscale = Scale;
	texindex = Texture;
	thick = Thickness;
}