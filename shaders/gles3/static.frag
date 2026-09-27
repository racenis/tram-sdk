// TRAMWAY DRIFT AND DUNGEON EXPLORATION SIMULATOR 2022
// All rights reserved.

precision highp float;
precision mediump sampler2DArray;

out vec4 fragment;
  
in vec2 vert_uv;
in vec2 vert_light_uv;
in vec3 vert_color;
in float vert_opacity;
flat in uint vert_tex_index;

uniform sampler2D sampler[15];
uniform sampler2DArray samplerArray;

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
	}
	
    fragment *= texture(samplerArray, vec3(vert_light_uv, 0.0)) * vec4(vert_color, vert_opacity);
#ifdef FLAG_ALPHA_TEST
	if (fragment.a < 0.5) discard;
#endif
}