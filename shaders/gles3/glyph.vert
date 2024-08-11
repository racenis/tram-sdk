#version 300 es

// TRAMWAY DRIFT AND DUNGEON EXPLORATION SIMULATOR 2022
// All rights reserved.

precision highp float;

layout (location = 0) in vec3 Position;		// vertex position
layout (location = 1) in vec2 TexCoord;		// texture coordinate
layout (location = 2) in vec3 VertColor;	// vertex color
layout (location = 5) in uint Texture;		// texture index

out vec2 vert_uv;
out vec3 vert_color;
flat out uint vert_tex_index;

uniform sampler2D sampler[16];

layout (std140) uniform ModelMatrices {
    mat4 model;
	uvec4 model_lights;
	vec3 sun_direction;
	vec3 sun_color;
	vec4 ambient_color;
	float time;
	float sun_weight;
	float screen_width;
	float screen_height;
	vec4 colors[15];
	vec4 specular[15];
};

void main() {
	// the Position variable actually contains the position in normal GUI
	// coordinates, where each unit is one pixel and the origin is in the
	// upper left corner.

	// we first need to convert these coordinates to OpenGL coordinates
	float pos_x = (round(Position.x) / (screen_width / 2.0)) - 1.0;
	float pos_y = (round(Position.y) / (screen_height / -2.0)) + 1.0;

	// then we compute the depth for correct ordering
	float depth = -0.5 - (Position.z / 128.0);
	
	// then we can output these coordinates
    gl_Position = vec4(pos_x, pos_y, depth, 1.0);
	
	// and scale the UVs based on the texture size
	ivec2 tex_size;
	
	switch (Texture) {
	case 0u:
		tex_size = textureSize(sampler[0], 0);
		break;
	case 1u:
		tex_size = textureSize(sampler[1], 0);
		break;
	case 2u:
		tex_size = textureSize(sampler[2], 0);
		break;
	case 3u:
		tex_size = textureSize(sampler[3], 0);
		break;
	case 4u:
		tex_size = textureSize(sampler[4], 0);
		break;
	case 5u:
		tex_size = textureSize(sampler[5], 0);
		break;
	case 6u:
		tex_size = textureSize(sampler[6], 0);
		break;
	case 7u:
		tex_size = textureSize(sampler[7], 0);
		break;
	case 8u:
		tex_size = textureSize(sampler[8], 0);
		break;
	case 9u:
		tex_size = textureSize(sampler[9], 0);
		break;
	case 10u:
		tex_size = textureSize(sampler[10], 0);
		break;
	case 11u:
		tex_size = textureSize(sampler[11], 0);
		break;
	case 12u:
		tex_size = textureSize(sampler[12], 0);
		break;
	case 13u:
		tex_size = textureSize(sampler[13], 0);
		break;
	case 14u:
		tex_size = textureSize(sampler[14], 0);
		break;
	case 15u:
		tex_size = textureSize(sampler[15], 0);
		break;			
	}
	
	
	vert_uv = vec2(TexCoord.x/float(tex_size.x), TexCoord.y/float(-tex_size.y));
		
	vert_color = VertColor;
	vert_tex_index = Texture;
}