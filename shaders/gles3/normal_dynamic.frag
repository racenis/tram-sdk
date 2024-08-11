#version 300 es

// TRAMWAY DRIFT AND DUNGEON EXPLORATION SIMULATOR 2022
// All rights reserved.

precision highp float;

out vec4 fragment;

in vec2 vert_uv;
in vec3 vert_color;
flat in uint vert_tex_index;

uniform sampler2D sampler[16];

void main() {
	vec4 texcolor;
	
	switch (vert_tex_index) {
	case 0u:
		texcolor = texture(sampler[0], vert_uv);
		break;
	case 1u:
		texcolor = texture(sampler[1], vert_uv);
		break;
	case 2u:
		texcolor = texture(sampler[2], vert_uv);
		break;
	case 3u:
		texcolor = texture(sampler[3], vert_uv);
		break;
	case 4u:
		texcolor = texture(sampler[4], vert_uv);
		break;
	case 5u:
		texcolor = texture(sampler[5], vert_uv);
		break;
	case 6u:
		texcolor = texture(sampler[6], vert_uv);
		break;
	case 7u:
		texcolor = texture(sampler[7], vert_uv);
		break;
	case 8u:
		texcolor = texture(sampler[8], vert_uv);
		break;
	case 9u:
		texcolor = texture(sampler[9], vert_uv);
		break;
	case 10u:
		texcolor = texture(sampler[10], vert_uv);
		break;
	case 11u:
		texcolor = texture(sampler[11], vert_uv);
		break;
	case 12u:
		texcolor = texture(sampler[12], vert_uv);
		break;
	case 13u:
		texcolor = texture(sampler[13], vert_uv);
		break;
	case 14u:
		texcolor = texture(sampler[14], vert_uv);
		break;
	case 15u:
		texcolor = texture(sampler[15], vert_uv);
		break;			
	}

    fragment = texcolor * vec4(vert_color, 1.0);
}