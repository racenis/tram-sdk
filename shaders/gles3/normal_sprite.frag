#version 300 es

// TRAMWAY DRIFT AND DUNGEON EXPLORATION SIMULATOR 2022
// All rights reserved.

precision highp float;

out vec4 fragment;
  
in vec2 vert_uv;
flat in uint vert_tex_index;

uniform sampler2D sampler[16];

void main() {
    
	switch (vert_tex_index) {
	case 0u:
		fragment = texture(sampler[0], vert_uv);
		break;
	case 1u:
		fragment = texture(sampler[1], vert_uv);
		break;
	case 2u:
		fragment = texture(sampler[2], vert_uv);
		break;
	case 3u:
		fragment = texture(sampler[3], vert_uv);
		break;
	case 4u:
		fragment = texture(sampler[4], vert_uv);
		break;
	case 5u:
		fragment = texture(sampler[5], vert_uv);
		break;
	case 6u:
		fragment = texture(sampler[6], vert_uv);
		break;
	case 7u:
		fragment = texture(sampler[7], vert_uv);
		break;
	case 8u:
		fragment = texture(sampler[8], vert_uv);
		break;
	case 9u:
		fragment = texture(sampler[9], vert_uv);
		break;
	case 10u:
		fragment = texture(sampler[10], vert_uv);
		break;
	case 11u:
		fragment = texture(sampler[11], vert_uv);
		break;
	case 12u:
		fragment = texture(sampler[12], vert_uv);
		break;
	case 13u:
		fragment = texture(sampler[13], vert_uv);
		break;
	case 14u:
		fragment = texture(sampler[14], vert_uv);
		break;
	case 15u:
		fragment = texture(sampler[15], vert_uv);
		break;			
	}
	
	if (fragment.w < 0.9) discard;
}