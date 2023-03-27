#version 400 core
out vec4 fragColor;

in vec2 vertUV;
in vec2 vertLightUV;
in vec3 vertColor;
flat in uint texIndex;

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

uniform sampler2D sampler[16];

void main()
{
    float offse = mod(time / 5.0, 1.0);
	vec2 uv = vec2(vertUV.x + offse, vertUV.y + offse);
	//vec2 uv2 = vec2(vertUV.x, vertUV.y + mod(time.x / 60.0, 1.0));
    fragColor = texture(sampler[texIndex], uv)/* * texture(sampler[texIndex], uv2) */* texture(sampler[15], vertLightUV) * vec4(vertColor, 1.0);
	// TODO: break the shader again and make water pretty
}
