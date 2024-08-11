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
	vec4 sampled_color;

	switch (vert_tex_index) {
	case 0u:
		sampled_color = texture(sampler[0], vert_uv);
		break;
	case 1u:
		sampled_color = texture(sampler[1], vert_uv);
		break;
	case 2u:
		sampled_color = texture(sampler[2], vert_uv);
		break;
	case 3u:
		sampled_color = texture(sampler[3], vert_uv);
		break;
	case 4u:
		sampled_color = texture(sampler[4], vert_uv);
		break;
	case 5u:
		sampled_color = texture(sampler[5], vert_uv);
		break;
	case 6u:
		sampled_color = texture(sampler[6], vert_uv);
		break;
	case 7u:
		sampled_color = texture(sampler[7], vert_uv);
		break;
	case 8u:
		sampled_color = texture(sampler[8], vert_uv);
		break;
	case 9u:
		sampled_color = texture(sampler[9], vert_uv);
		break;
	case 10u:
		sampled_color = texture(sampler[10], vert_uv);
		break;
	case 11u:
		sampled_color = texture(sampler[11], vert_uv);
		break;
	case 12u:
		sampled_color = texture(sampler[12], vert_uv);
		break;
	case 13u:
		sampled_color = texture(sampler[13], vert_uv);
		break;
	case 14u:
		sampled_color = texture(sampler[14], vert_uv);
		break;
	case 15u:
		sampled_color = texture(sampler[15], vert_uv);
		break;			
	}

	
	
	if (sampled_color.w < 1.0) discard;
	
	fragment = sampled_color * vec4(vert_color, 1.0);
}
