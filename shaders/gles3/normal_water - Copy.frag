#version 300 es

// TRAMWAY DRIFT AND DUNGEON EXPLORATION SIMULATOR 2022
// All rights reserved.

precision highp float;

out vec4 fragment;

in vec2 vert_uv;
in vec2 vert_light_uv;
in vec3 vert_color;
flat in uint vert_tex_index;

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
	vec2 specular[15];
};

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


    float offse = mod(time / 5.0, 1.0);
	vec2 uv = vec2(vert_uv.x + offse, vert_uv.y + offse);
	//vec2 uv2 = vec2(vert_uv.x, vert_uv.y + mod(time.x / 60.0, 1.0));
    fragment = texcolor/* * texture(sampler[vert_tex_index], uv2) */* texture(sampler[15], vert_light_uv) * vec4(vert_color, 1.0);
	// TODO: break the shader again and make water pretty
}
