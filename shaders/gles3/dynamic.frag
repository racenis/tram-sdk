// TRAMWAY DRIFT AND DUNGEON EXPLORATION SIMULATOR 2022
// All rights reserved.

precision highp float;
precision mediump sampler2DArray;

out vec4 fragment;

in vec2 vert_uv;
in vec3 vert_color;
in vec3 vert_color_add;
in vec3 vert_reflection;
in float vert_opacity;
in float vert_reflectivity;
flat in uint vert_tex_index;

uniform sampler2D sampler[15];
uniform sampler2DArray samplerArray;

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
	}

    fragment = texcolor * vec4(vert_color, 1.0);
	
	fragment += vec4(vert_color_add, 0.0);
	
#ifdef FLAG_ALPHA_TEST
	if (fragment.a < 0.5) discard;
#endif
	
	// TODO: fix sampling
	/*
	vec3 reflection_coords;
	if (vert_reflection.z > 0.0) {
		reflection_coords.xy = 0.5 + 0.5 * (vert_reflection.xy / (1.0 + vert_reflection.z));
		reflection_coords.z = 1.0;
	} else {
		reflection_coords.xy = 0.5 + 0.5 * (vert_reflection.xy / (1.0 - vert_reflection.z));
		reflection_coords.z = 0.0;
	}
	
	vec3 reflection_color = vert_reflectivity * vec3(texture(samplerArray, reflection_coords));
	float reflection_brightness = dot(vec3(0.299, 0.587, 0.114), vec3(reflection_color));
	
	fragment += vec4(reflection_color, reflection_brightness);*/
}