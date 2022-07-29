#version 400 core
layout (location = 0) in vec3 Position;
layout (location = 1) in vec3 Normal;
layout (location = 2) in vec2 VertUV;
layout (location = 3) in ivec4 BoneIndex;
layout (location = 4) in vec4 BoneWeight;
layout (location = 5) in uint TexIndex;

struct Light
{
	vec4 aa;
	vec4 bb;
	vec4 cc;
	vec4 dd;
};

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

layout (std140) uniform Lights
{
	Light scenelights[50];
};

layout (std140) uniform Bones
{
	mat4 bone[30];
};

out vec3 vertColor;
out vec2 vertUV;
flat out uint texIndex;




void main()
{
	vec3 lightColor = ambientcolor * time.y;
	mat4 pvm = projection * view * model;
	vec4 bonepose1 = pvm * bone[BoneIndex.x] * vec4(Position, 1.0);
	vec4 bonepose2 = pvm * bone[BoneIndex.y] * vec4(Position, 1.0);
	vec4 bonepose3 = pvm * bone[BoneIndex.z] * vec4(Position, 1.0);
	vec4 bonepose4 = pvm * bone[BoneIndex.w] * vec4(Position, 1.0);
	
    gl_Position = bonepose1 * BoneWeight.x + bonepose2 * BoneWeight.y + bonepose3 * BoneWeight.z + bonepose4 * BoneWeight.w;
	vec3 vPos = vec3(model * vec4(Position, 1.0));
	vec3 nPos = normalize(vec3(model * vec4(Normal, 0.0)));	//set this to 0.0
	float distance1 = length(vec3(scenelights[modellights.x].aa) - vPos);
	float distance2 = length(vec3(scenelights[modellights.y].aa) - vPos);
	float distance3 = length(vec3(scenelights[modellights.z].aa) - vPos);
	float distance4 = length(vec3(scenelights[modellights.w].aa) - vPos);
	
	lightColor += suncolor * max(dot(nPos, normalize(sundir)), 0.0) * time.y;
	
	lightColor += vec3(scenelights[modellights.x].bb) * max(dot(nPos, normalize(vec3(scenelights[modellights.x].aa) - vPos)), 0.0) * (1.0 / (1.0 + 0.09 * distance1 + 0.032 * (distance1 * distance1)));
	lightColor += vec3(scenelights[modellights.y].bb) * max(dot(nPos, normalize(vec3(scenelights[modellights.y].aa) - vPos)), 0.0)* (1.0 / (1.0 + 0.09 * distance2 + 0.032 * (distance2 * distance2)));
	lightColor += vec3(scenelights[modellights.z].bb) * max(dot(nPos, normalize(vec3(scenelights[modellights.z].aa) - vPos)), 0.0)* (1.0 / (1.0 + 0.09 * distance3 + 0.032 * (distance3 * distance3)));
	lightColor += vec3(scenelights[modellights.w].bb) * max(dot(nPos, normalize(vec3(scenelights[modellights.w].aa) - vPos)), 0.0)* (1.0 / (1.0 + 0.09 * distance4 + 0.032 * (distance4 * distance4)));
	
	vertColor = lightColor;
    vertUV = VertUV;
	texIndex = TexIndex;
}